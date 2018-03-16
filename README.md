<!-- MarkdownTOC -->

- [CLI Tool to transcode measurement data](#cli-tool-to-transcode-measurement-data)
  - [Usage](#usage)
  - [How to build](#how-to-build)
  - [How to use example](#how-to-use-example)
  - [License](#license)
  - [Acknowledgments](#acknowledgments)

<!-- /MarkdownTOC -->

# CLI Tool to transcode measurement data

Climgal (**C**ommand**LI**ne **M**easurement processin**G** **A**pp**L**ication) is an open source tool to read and write different types of data used by measurement soft- and hardware.
The tool utilize the [readerlib](https://github.com/MeasureTools/readerlib).

[![Build Status](https://travis-ci.org/MeasureTools/climgal.svg?branch=master)](https://travis-ci.org/MeasureTools/climgal)

## Usage

```
Climgal - CommandLIne Measurement processinG AppLication

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
```

If a source format doesn't support out of the box a fixed resolution, a resolution needs to be given. The tool outputs the new measurement via standard output which allows easy handling with other tools.

## How to build

```bash
git clone --recursive https://github.com/MeasureTools/climgal.git climgal
cd climgal
mkdir build
cd build
cmake ..
make
```

## How to use example

```bash
climgal --file=example.csv --begin=10 --end=20 --resolution=1 --format=dlog > output.dlog
```
In this example, the measurement *example.csv* is our **source**. Only samples between 10s (begin) and 20s (end) are used to write the new measurement data. Also the **resolution** is set to 1Hz. If the source measurement has a higher or lower resolution, the data is resampled according to the new samplerate. As output format **dlog** was chosen which is a format used by Keysight. Finally the new measurement data is saved to the file *output.dlog*.

## License

This project is licensed under a modified BSD 1-Clause License with an additional non-military use clause - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

* TU Dortmund [Embedded System Software Group](https://ess.cs.tu-dortmund.de/EN/Home/index.html) which made this release possible
