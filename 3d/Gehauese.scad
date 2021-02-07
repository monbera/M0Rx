$fa = 12;
$fs = 0.5;

// gehauese
//rotate([0,90,0])
{
difference(){
cube([55.8,25.2,23.2], center = true);
// inner cube    
translate([-1,0,0])
    cube([55.8,23.2,21.2], center = true);  
// antenna hole
translate([-23.4,-10,8])
    rotate([90,0,0])
    cylinder(h=4,r=1);  
// hole for servo pins  
translate([26.9,1.56,-3.7])    
    cube ([4, 10.5, 8], center = true); 
// hole for USB 
translate([26.9, 0, 5.7])
    cube ([5, 10, 4], center = true);  
// hole for battery
translate([15.9,-12, 6.5])
    cube ([8, 3, 5], center = true);   
 
// rest button
translate([16.8,5.1, 10])
    cylinder(h=3, r=1);     
}} 