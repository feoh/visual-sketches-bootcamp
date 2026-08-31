# Section 17 reference solution: two-light archipelago

This answer uses nine parent clusters and 1,180 smaller forms. A directional warm
light and cool point light cross the mass; material shininess differs between
the two inherited families. Slow reverse rotation and a farther camera produce a
more dispersed silhouette than the starter.

The pure model knows nothing about openFrameworks. It generates finite transform
and material records from explicit settings and a seed. The renderer owns the
camera, primitive, depth state, lights, and materials. That boundary lets tests
check hierarchy and caps without pretending to judge highlights.

Build with `scripts/section-17.sh generate --project solution` and
`scripts/section-17.sh build --project solution --configuration Release` (or
the matching PowerShell commands). Change clusters, voids, silhouette rhythm,
and light placement rather than copying this scene.
