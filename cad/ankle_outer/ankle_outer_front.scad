// ankle front
// measures are in mm
// copied and adapted from the blueprints of CS:R from R2 Builders Club

h = 2;
c = 1;

module triangle(l, w, h) {
  CubePoints = [
    [0, 0, 0], // 0
    [l, 0, 0], // 1
    [l, w, 0], // 2
    [0, 0, h], // 3
    [l, 0, h], // 4
    [l, w, h], // 5
    ];

  CubeFaces = [
    [0, 1, 2], // bottom
    [0, 3, 4, 1], // front
    [5, 4, 3], // top
    [1, 4, 5, 2], // right
    [2, 5, 3, 0], // back, left
    ]; // left

  polyhedron(CubePoints, CubeFaces);
}

module the_triangle() {
  translate([-64.5/2, 4.3+35+17.7-0.02, 0])
    triangle(64.5/2-8.7/2, 7, h);
}

module outer() {
  intersection() {
    translate([-107.5/2,0,0])
      cube(size=[107.5,76.5,h]);

    cylinder(h, 77, 77);
  }
}

module inner() {
  translate([0, 62, 0])
    cylinder(h, 8.7, 8.7);

  translate([-87/2, 4.3, 0])
    cube(size=[87, 35, h]);

  translate([-64.5/2, 4.3+35-0.01, 0])
    cube(size=[64.5, 17.7, h]);

  the_triangle();
  mirror([1, 0, 0])
    the_triangle();
}

module all() {
  difference() {
    outer();
    inner();
  }
}

all();
