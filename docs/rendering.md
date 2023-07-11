# Rendering in *Mandalin*
## Overview
 Due to the number of hexes we hope to render each frame
 and the filters we need to implement so that the player can
 view different features of the world, we need to have a
 somewhat sophisticated rendering system in place. We can't
 get away with just throwing all our hexes at the GPU or
 naively chunking the planet and hoping for the best. In
 addition, we need to think ahead; we are eventually going
 to add multiplayer support, so the way we approach
 rendering now will have repercussions down the line.

 The world will be broken down into chunks based on whether
 it is composed of land or water tiles; each chunk will
 include all the geometry of its respective hexes. Each
 frame, we will render those chunks that are visible to
 the player, beginning with land chunks and then moving on
 to water chunks. This distinction is important as we will
 need to apply different shaders to land chunks and water
 chunks, though it will make our rendering system slightly
 more complicated than it would otherwise be.

## Temporary Notes
 * Different shader batches for different lenses
 (geography, political, ethnicity, religion, etc.)