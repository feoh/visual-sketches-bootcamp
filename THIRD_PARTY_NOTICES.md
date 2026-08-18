# Third-party notices

This file records bundled code patterns whose terms are more specific than the
repository's default scopes. Linked documentation and dependencies are not
bundled or relicensed merely because they are cited.

## Boost hash-combine pattern

`exercises/11-noise-and-flow-fields/shared/flow_field_model.cpp` uses the
hash-combine expression documented by the Boost.ContainerHash contributors:

- Source: Boost 1.55, “Combining hash values,” as recorded in
  `authoring/sections/11-noise-and-flow-fields/sources.yaml`
- Copyright: Boost authors and contributors
- License: Boost Software License 1.0 (BSL-1.0)
- Full license text: `LICENSES/BSL-1.0.txt`

The surrounding coordinate/seed composition, bounds, and value-noise pipeline
are course code. Retain this notice and the BSL-1.0 text when redistributing the
bundled expression.

## Chris Wellons lowbias32 finalizer

The same function uses constants and operations from Chris Wellons's
`lowbias32` work, cited in the section-11 source record. The author's site
releases its source-code examples to the public domain. That public-domain
provenance is preserved here and in the lesson; it is not relicensed under MIT
or CC BY 4.0.
