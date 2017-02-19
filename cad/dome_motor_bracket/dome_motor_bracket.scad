// JohK nutsnbolts library
include <../nutsnbolts/cyl_head_bolt.scad>

// all measures in mm
d = 10;
thickness = 2;
do = d+2*thickness;
thickness_mount = 4; // additional to thickness
h = 60+thickness;
clr = 0.3; // clearance of cutouts
screw = "M4";

module holder() {
     // make the half of the bracket square to lay flat on the thing to mount to
     cube(size=[do,do/2,h]);

     // the rest round to spare filament
     translate([do/2,do/2,0])
          cylinder(h,do/2,do/2);
}

module shaft_hole() {
     translate([do/2,do/2,thickness])
          cylinder(h,d/2+clr,d/2+clr);
     translate([-clr,0,10])
          cube(size=[do+2*clr,do+clr,h-2*10]);
}

module base() {
     difference() {
          cube(size=[do,thickness_mount,h]);

          // nut-catch for mounting bracket
          translate([do/2,5,8]) rotate([-90,0,0])
               nutcatch_parallel(screw, l=20, clk=clr, clh=clr, clsl=clr);
          translate([do/2,5,h-8]) rotate([-90,0,0])
               nutcatch_parallel(screw, l=20, clk=clr, clh=clr, clsl=clr);
     }

     translate([0,thickness_mount,0])
          difference() {
          holder();
          shaft_hole();
     }
}

module shaft() {
     difference() {
          translate([0,0,thickness])
               cylinder(h-thickness,d/2,d/2);

          // nut-catch for mounting motor
          translate([0,0,18]) rotate([90,0,0])
               nutcatch_parallel(screw, l=20, clk=clr, clh=clr, clsl=clr);
          translate([0,0,h-18]) rotate([90,0,0])
               nutcatch_parallel(screw, l=20, clk=clr, clh=clr, clsl=clr);
     }
}

{
     color("red")
          base();

     translate([do/2,do/2+thickness_mount,0])
          shaft();
}
