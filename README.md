# Quantum Script Extension Base64

Quantum Script extension
- Base64 (RFC 4648) encoding of strings and buffers:
`Base64.encode` writes every 3 bytes as 4 characters from
`A-Z a-z 0-9 + /`, padded with `=`.
- Strict decoding back to a `String` or a `Buffer`: `Base64.decode`,
`Base64.decodeToBuffer` return `undefined` on invalid input.
- For putting binary data into text: JSON and XML payloads, MIME and PEM,
`data:` URLs, HTTP `Basic` authorization, web APIs.

```javascript
Script.requireExtension("Base64");

Base64;
Base64.encode(str);
Base64.decode(str);
Base64.decodeToBuffer(str);
```

Built on `quantum-script` and `quantum-script--buffer`, part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script, hosts, register in a C++ host
- [Script API](docs/script-api.md) - every function: format, exact validation, edge cases, recipes
- [C++ API](docs/cpp-api.md) - registration, DLL entry point, the codec in `xyo-encoding`
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--base64](.claude/skills/quantum-script--base64/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
