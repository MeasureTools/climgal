/**
 * Copyright (c) 2016-2017, Daniel "Dadie" Korner
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Neither the source code nor the binary may be used for any military use.
 *
 * THIS SOFTWARE IS PROVIDED BY Daniel "Dadie" Korner ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Daniel "Dadie" Korner BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **/

// Extern
#include <docopt.h>

// Own
#include <rlib/android/meta_reader.h>
#include <rlib/common/reader.h>
#include <rlib/csv/csv_exporter.h>
#include <rlib/csv/csv_reader.h>
#include <rlib/grim/grim_reader.h>
#include <rlib/keysight/dlog_exporter.h>
#include <rlib/keysight/dlog_reader.h>
#include <rlib/powerscale/psi_reader.h>
#include <rlib/svg/svg_exporter.h>
#include <rlib/xml/xml_exporter.h>
#include <rlib/xml/xml_reader.h>

// StdLib
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <tuple>
#include <utility>

static const char USAGE[] =
    R"(Climgal - CommandLIne Measurement processinG AppLication

    Usage:
      climgal --file=<file> [--begin=<time>] [--end=<time>] [--resolution=<hz>] [--format=<format>]
      climgal (-h | --help)
      climgal --version

    Options:
      -h --help          Show this screen.
      --version          Show version.
      --file=<file>      Input file (Supported Formats: .meta, .grim, .dlog, .psi, .csv, .xml)
      --begin=<time>     Output only data after the begin time (in seconds) [default: 0.0]
      --end=<time>       Output only data befor the end time (in seconds). If end < begin, ignore end [default: -1.0]
      --resolution=<hz>  Resolution in Hz. If resolution <= 0, ignore it [default: -1]
      --format=<format>  Output format (Supported Formats: csv, xml, dlog, svg). [default: xml]
)";

int main(int argc, char* argv[])
{
    std::map< std::string, docopt::value > args =
        docopt::docopt(USAGE, { argv + 1, argv + argc },
            true,             // show help if requested
            "climgal 0.1.0"); // version string

    // Validate paramters
    if (!args[ "--file" ].isString()) {
        std::cerr << "ERROR given file parameter is not a string" << std::endl;
        return -1;
    }

    if (!args[ "--file" ].isString()) {
        std::cerr << "ERROR given file parameter is not a string" << std::endl;
        return -1;
    }
    if (!args[ "--format" ].isString()) {
        std::cerr << "ERROR given file parameter is not a string" << std::endl;
        return -1;
    }
    try {
        std::stod(args[ "--begin" ].asString());
    }
    catch (const std::invalid_argument& ie) {
        std::cerr << "ERROR given begin parameter is not a number" << std::endl;
        return -1;
    }
    try {
        std::stod(args[ "--end" ].asString());
    }
    catch (const std::invalid_argument& ie) {
        std::cerr << "ERROR given end parameter is not a number" << std::endl;
        return -1;
    }
    try {
        args[ "--resolution" ].asLong();
    }
    catch (std::runtime_error e) {
        std::cerr << "ERROR given resolution parameter is not an integer"
                  << std::endl;
        return -1;
    }

    // Initialize reader
    std::map< std::string,
        std::function< std::shared_ptr< rlib::common::reader >(std::string) > >
        reader;
    {
        reader.insert_or_assign(".dlog", [](std::string file) {
            return std::make_shared< rlib::keysight::dlog_reader >(file);
        });
        reader.insert_or_assign(".psi", [](std::string file) {
            return std::make_shared< rlib::powerscale::psi_reader >(file);
        });
        reader.insert_or_assign(".meta", [](std::string file) {
            return std::make_shared< rlib::android::meta_reader >(file);
        });
        reader.insert_or_assign(".grim", [](std::string file) {
            return std::make_shared< rlib::grim::grim_reader >(file);
        });
        reader.insert_or_assign(".csv", [](std::string file) {
            return std::make_shared< rlib::csv::csv_reader >(file);
        });
        reader.insert_or_assign(".xml", [](std::string file) {
            return std::make_shared< rlib::xml::xml_reader >(file);
        });
    }
    std::shared_ptr< rlib::common::reader > fileReader;
    bool foundReader = false;
    for (auto& tuple : reader) {
        auto ext = std::get< 0 >(tuple);
        if (args[ "--file" ].isString() &&
            args[ "--file" ].asString().size() >= ext.size()) {
            if (args[ "--file" ].asString().compare(
                    args[ "--file" ].asString().size() - ext.size(), ext.size(),
                    ext) == 0) {
                fileReader = std::get< 1 >(tuple)(args[ "--file" ].asString());
                foundReader = true;
                break;
            }
        }
    }
    if (!foundReader) {
        std::cerr << "ERROR Missing File: " << args[ "--file" ].asString()
                  << std::endl;
        return -1;
    }

    for (auto& sensor : fileReader->sensors()) {
        if (sensor.sampling_interval <= 0 &&
            args[ "--resolution" ].asLong() <= 0) {
            std::cerr << "ERROR The Format " << args[ "--file" ].asString()
                      << " has no information about the resolution. Please set "
                         "--resolution."
                      << std::endl;
            return -1;
        }
    }

    // Initialize format function
    std::map< std::string,
        std::function< void(std::shared_ptr< rlib::common::reader >, double,
            double, int_fast32_t, std::ostream&) > >
        formater;
    {
        formater.insert_or_assign("csv",
            [](std::shared_ptr< rlib::common::reader > reader, double begin,
                double end, int_fast32_t resolution, std::ostream& output) {
                rlib::csv::csv_exporter(reader).data_export(
                    begin, end, resolution, output);
            });
        formater.insert_or_assign("xml",
            [](std::shared_ptr< rlib::common::reader > reader, double begin,
                double end, int_fast32_t resolution, std::ostream& output) {
                rlib::xml::xml_exporter(reader).data_export(
                    begin, end, resolution, output);
            });
        formater.insert_or_assign("dlog",
            [](std::shared_ptr< rlib::common::reader > reader, double begin,
                double end, int_fast32_t resolution, std::ostream& output) {
                rlib::keysight::dlog_exporter(reader).data_export(
                    begin, end, resolution, output);
            });
        formater.insert_or_assign("svg",
            [](std::shared_ptr< rlib::common::reader > reader, double begin,
                double end, int_fast32_t resolution, std::ostream& output) {
                rlib::svg::svg_exporter(reader).data_export(
                    begin, end, resolution, output);
            });
    }
    std::function< std::string(std::shared_ptr< rlib::common::reader >, double,
        double, int32_t, std::ostream&) >
        formFunction;
    bool foundFormater = false;
    for (auto& tuple : formater) {
        auto type = std::get< 0 >(tuple);
        if (args[ "--format" ].isString() &&
            type == args[ "--format" ].asString()) {
            foundFormater = true;
            break;
        }
    }
    if (!foundFormater) {
        std::cerr << "ERROR Unknown formater " << args[ "--format" ].asString()
                  << std::endl;
        return -1;
    }

    double begin = std::stod(args[ "--begin" ].asString());
    double end = std::stod(args[ "--end" ].asString());
    int_fast32_t resolution = int_fast32_t(args[ "--resolution" ].asLong());
    formater[ args[ "--format" ].asString() ](
        fileReader, begin, end, resolution, std::cout);
    return 0;
}
