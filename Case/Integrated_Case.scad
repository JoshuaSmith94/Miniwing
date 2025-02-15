// Keyboard Case Template with Structural Support Ribs

// Base parameters
pcb_thickness = 1.6;  
wall_thickness = 1.5;
base_thickness = 1.5;

// Support rib parameters
rib_height = 1.2;
rib_width = 2;

// Post parameters
post_height = 5;
hole_diameter = 4.32;
post_diameter = hole_diameter + 3.2;
case_height = base_thickness + pcb_thickness + post_height;

// USB cutout dimensions
usb_width = 14;
usb_height = 8;
usb_x_pos = -7;
usb_y_pos = 2;

// Function to create a mounting post
module mounting_post(point) {
    translate(point) {
        difference() {
            cylinder(h=post_height+base_thickness, d=post_diameter, $fn=64);
            translate([0, 0, -1])
                cylinder(h=post_height + base_thickness + 2, d=hole_diameter, $fn=64);
        }
    }
}

// USB cutout module
module usb_cutout() {
    translate([usb_x_pos, usb_y_pos, base_thickness+rib_height])
        cube([usb_width, wall_thickness + 2, usb_height]);
}

// Support ribs network module
module support_rib(point1, point2) {
    // Calculate the angle between the points
    angle = atan2(point2[1] - point1[1], point2[0] - point1[0]);
    
    // Calculate the length between points using pythagoras
    length = sqrt(pow(point2[0] - point1[0], 2) + pow(point2[1] - point1[1], 2));
    
    // Translate to first point and rotate
    translate(point1)
    rotate([0, 0, angle])
    // Create the rectangle centered on the x-axis
    translate([length/2, 0, rib_height/2 + base_thickness])
    cube([length-hole_diameter, rib_width, rib_height], center=true);
}


// Main case assembly
module main_case() {
    posts = [[-22.458, 1.507, 0],
             [22.332, 1.473, 0],
             [72.996, -1.512, 0],
             [-72.943, -1.421, 0],
             [-46.430, -33.543, 0],
             [46.339, -33.543, 0],
             [-69.660, -39.404, 0],
             [69.864, -39.404, 0],
             [-17.703, -61.099, 0],
             [17.074, -61.099, 0]];
                
    difference() {
        union() {
            // Outer case shell
            linear_extrude(height=case_height)
                offset(r=wall_thickness) 
                    import("miniwing.dxf");
            
            // Base plate
            linear_extrude(height=base_thickness)
                offset(r=wall_thickness) 
                    import("miniwing.dxf");
        }
        
        // Hollow out the inside
        translate([0, 0, base_thickness])
            linear_extrude(height=case_height)
                import("miniwing.dxf");
        
        // Add USB cutout
        usb_cutout();
    }
    // Mounting posts
    for (i = [0:len(posts)-1]) {
        mounting_post(posts[i]);
    }
    
    // Add support ribs
    support_rib(posts[0],posts[3]);
    support_rib(posts[0],posts[4]);
    support_rib(posts[0],posts[8]);
    support_rib(posts[0],posts[9]);
    support_rib(posts[1],posts[2]);
    support_rib(posts[1],posts[5]);
    support_rib(posts[1],posts[8]);
    support_rib(posts[1],posts[9]);
    support_rib(posts[2],posts[5]);
    support_rib(posts[2],posts[7]);
    support_rib(posts[3],posts[4]);
    support_rib(posts[3],posts[6]);
    support_rib(posts[4],posts[5]);
    support_rib(posts[4],posts[6]);
    support_rib(posts[4],posts[8]);
    support_rib(posts[5],posts[7]);
    support_rib(posts[5],posts[9]);
    support_rib(posts[6],posts[8]);
    support_rib(posts[7],posts[9]);
    support_rib(posts[8],posts[9]);

    support_rib(posts[0],[3,3,0]);
    support_rib(posts[1],[-3,3,0]);
    support_rib(posts[0],[-20,9,0]);
    support_rib(posts[1],[20,9,0]);
    support_rib(posts[3],[-92,-22,0]);
    support_rib(posts[6],[-92,-18,0]);
    support_rib(posts[6],[-72,-48,0]);
    support_rib(posts[2],[92,-22,0]);
    support_rib(posts[7],[92,-18,0]);
    support_rib(posts[7],[72,-48,0]);
    support_rib(posts[8],[-15,-68,0]);
    support_rib(posts[9],[15,-68,0]);
}

// Render the case
main_case();

/* 
Usage Instructions:
1. Export your PCB outline from KiCad Edge.Cuts layer as DXF
2. Save the DXF as "miniwing.dxf" in the same directory as this file
3. Adjust support rib parameters if needed:
   - rib_height: Height of the support ribs (2.0mm default)
   - rib_width: Width of the support ribs (2.0mm default)
4. Preview with F5, render with F6
*/