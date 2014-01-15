#!/usr/bin/env python2.7
#
# Triangle Analyzer tests
#
# Student: John Clukey
# Date: January 13, 2014

import subprocess

# Some test points. Expected result followed by coordinates.
testpts = [["scalene obtuse\n", "0", "0", "2147483647", "2147483647", "2147483647", "2147483646"], 
["isosceles acute\n", "0", "0", "2147483646", "2147483647", "2147483647", "2147483646"], 
["not a triangle\n", "0", "0", "1073741823", "1073741823", "2147483646", "2147483646"], 
["scalene obtuse\n", "0", "0", "1073741822", "1073741824", "2147483645", "2147483647"], 
["isosceles obtuse\n", "0", "0", "1073741822", "1073741824", "2147483646", "2147483646"], 
["scalene acute\n", "1", "0", "0", "2147483647", "2147483647", "1"], 
["isosceles right\n", "1", "0", "0", "2147483646", "2147483647", "1"], 
["scalene right\n", "0", "0", "3", "0", "0", "5"], 
["scalene acute\n", "1", "2", "2", "7", "5", "3"], 
["not a triangle\n", "6", "6", "6", "6", "6", "6"]]


if __name__ == "__main__":
    passed = 0
    failed = 0
    for point in testpts:
        result = subprocess.check_output(["./triangle", 
                                          point[1], 
                                          point[2], 
                                          point[3], 
                                          point[4], 
                                          point[5], 
                                          point[6]])
        if result == point[0]:
            passed += 1
        else:
            failed += 1
            
    fd = open("tests.txt", "r")
    expected = fd.readline()
    while expected != "":
        pts = fd.readline().split()
        result = subprocess.check_output(["./triangle",
                                          pts[0],
                                          pts[1],
                                          pts[2],
                                          pts[3],
                                          pts[4],
                                          pts[5]])
        if expected == result:
            passed += 1
        else:
            failed += 1
        expected = fd.readline()
        
    print "Passed: " + str(passed) + " Failed: " + str(failed)