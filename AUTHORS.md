# Authors

tibiarc is free software under the GNU Affero General Public License,
version 3 or later (see [COPYING](COPYING)). Copyright belongs to the people
who wrote it; every source file names its holders in its header.

## Copyright holders

| Holder | Years | Work |
| --- | --- | --- |
| Silver Squirrel Software Handelsbolag | 2011–2016 | The original library, written for Tibiacast |
| John Högberg | 2023–2026 | Release as open source; support for arbitrary versions and recording formats; the command-line tools |
| Simon Sandström | 2024 | Build system |
| Juan Pinheiro | 2026 | This fork: restructuring into `core`, `client` and `apps`; the client that reproduces the game window and interface; the WebAssembly player and web front end; tests and documentation |

## About this fork

This is a modified version of [tibiarc](https://github.com/tibiacast/tibiarc),
changed from November 2026 onwards. The protocol parser, the recording
readers and the handling of data files come from the original project and
keep the notices of their authors; files changed here carry an additional
notice, and files new to the fork name only their own author.

Vendored code in `third_party/` and the sample recordings in `tests/` belong
to their respective authors under their own licenses; see the files
themselves and [tests/LICENSES.md](tests/LICENSES.md).

## Contributing

Add yourself to the header of the files you change substantially, as
`Copyright <year> "<name>"`, and to the table above.
