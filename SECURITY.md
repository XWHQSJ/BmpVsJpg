# Security Policy

## Supported Versions

| Version | Supported |
|---------|-----------|
| 1.x     | Yes       |

## Reporting a Vulnerability

If you find a security vulnerability in this project, please report it
responsibly:

1. **Do not** open a public GitHub issue for security vulnerabilities.
2. Email the maintainer directly or use GitHub's private vulnerability
   reporting feature (Security tab > Report a vulnerability).
3. Include a description of the issue, steps to reproduce, and any
   proof-of-concept input files.

We aim to acknowledge reports within 48 hours and provide a fix or
mitigation plan within 7 days.

## Security Measures

This project uses several automated security measures:

- **CodeQL** static analysis on every push and PR
- **Dependabot** for GitHub Actions dependency updates
- **libFuzzer** targets for the JPEG decoder and BMP reader, run in CI
  for 5 minutes per target on every push
- **AddressSanitizer** and **UndefinedBehaviorSanitizer** test matrix
  on Ubuntu and macOS

## Scope

Since `libjpeg_edu` wraps libjpeg-turbo for JPEG operations, vulnerabilities
in the JPEG decode/encode path may originate upstream. We still accept
reports for issues triggered through our API surface, including:

- Buffer overflows in BMP/PPM parsing (our own code)
- Integer overflows in dimension calculations
- Unchecked marker/header lengths
- Memory safety issues in filter or octree code
