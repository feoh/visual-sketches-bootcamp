# Explained reference solution

## What this example tries

The reference chooses a cool dark palette, focus `0.72`, and vertical bias
`0.35`. It keeps the shared responsive layout unchanged but draws an outlined
panel, three concentric orbit rings, a large triangle, and a negative-space focal disk.
The ring step is limited by one third of the available distance from the focus to the
nearest panel edge, so the outer orbit remains contained at every valid viewport,
including `64 x 64`. Outline, nesting, and overlap carry hierarchy without relying
only on color. This intentionally differs from the starter's filled panel, isolated dot,
and horizontal rule; make a third composition.
