// ankle back box
// measures are in mm
// copied and adapted from the blueprints of CS:R from R2 Builders Club

h = 2; // thickness of box
c = 0.01; // cut over edge

module prism(lb, wb, lt, wt, h) {
  ld = (lb - lt) / 2;
  wd = (wb - wt) / 2;

  points = [
    [0, 0, 0], // 0
    [lb, 0, 0], // 1
    [lb, wb, 0], // 2
    [0, wb, 0], // 3
    [ld, wd, h], // 4
    [lb-ld, wd, h], // 5
    [lb-ld, wb-wd, h], // 6
    [ld, wb-wd, h], // 7
    ];

  faces = [
    [0, 1, 2, 3], // bottom
    [0, 4, 5, 1], // front
    [7, 6, 5, 4], // top
    [1, 5, 6, 2], // right
    [2, 6, 7, 3], // back
    [3, 7, 4, 0], // left
    ];

  polyhedron(points, faces);
}

// outer dimensions of box
module outer() {
  cube(size=[100, 30, 106]);

  mirror([0,0,1])
    prism(100, 30, 85.7, 30, 9.2);
}

// inner dimensions of box
module inner() {
  translate([h, 5, 0])
    cube(size=[100-h*2, 25 + c, 106 + c]);

  translate([h, 5, c])
    mirror([0,0,1])
    prism(100-h*2, 25 + c, 85.7-h, 25 + c, 9.2-h + 2*c);
}

module all() {
  difference() {
    outer();
    inner();
  }
}

all();
