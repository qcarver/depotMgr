# depotMgr
Uses Aruco Fiducials to Identify location of depot bins

to use:
1) Aruco requires OpenCV (I used 4.5.2), so install OpenCV: https://riptutorial.com/opencv/example/15781/build-and-install-opencv-from-source
2) depotMgr was built w/ Aruco 3.1.9, so install Aruco: http://www.uco.es/
3) inside depotMgr folder mkdir bin
4) cd bin
5) cmake ../
6) make
7) ./depotMgr ../x.jpg ../arucoConfig.yml


Notes:
1) arucoConfig.yml is a configuration file to improve results. You should make your own, see aruco site for details
2) There is no two.

