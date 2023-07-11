# World Generation in *Mandalin*
## Geometry
 World generation is a pivotal part of *Mandalin* as it will allow
 for a more varied gameplay experience. While the initial placement
 of land and sea tiles will be random, as will be the designation of
 continents, we want all other information about the faultlines,
 currents, winds, and climate zones to arise deterministically.

 This process begins with the creation of the physical world.
 Beginning with an icosahedron, we "truncate" and "dual" the shape
 until we end up at the desired world-size; then, we truncate one
 last time. Each time we truncate, we find the midpoints between
 each vertex and its neighbors and treat these as the new vertices
 of the face of a hex; when we dual the shape, we take the center
 of each hex and treat it as the vertex of six new triangles
 whose other vertices are the centers of the neighbors of that
 hex. By truncating and dualing how ever many times we want and
 finishing with one final truncation, we end up with a world that
 is arbitrarily large.

 <p align="center"><img src="https://upload.wikimedia.org/wikipedia/commons/e/e7/G%C3%A9ode_V_3_1_duale.gif"
 alt="A hex-tiled sphere rotating in place."></p>

 We have to be smart about our implementation of this as we want
 it to be computationally efficient while also making it easy to
 calculate the neighbors of each hex and their respective chunks.
