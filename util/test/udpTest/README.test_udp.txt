Example Usage:

-Open two window, one is local machine(lab1), one is connected to host lab2

on local machine lab1:
-./bin.i686/test_udp lab2 9000 9001

on lab2 window:
-./bin.i686/test_udp lab1 9001 9000

======================================================================

Copyright 2000-2003, Cisco Systems, Inc.
THE INFORMATION HEREIN IS PROVIDED ON AN "AS IS" BASIS, WITHOUT ANY
WARRANTIES OR REPRESENTATIONS, EXPRESS, IMPLIED OR STATUTORY, INCLUDING
WITHOUT LIMITATION, WARRANTIES OF NONINFRINGEMENT, MERCHANTABILITY OR
FITNESS FOR A PARTICULAR PURPOSE.

$Id: README.test_udp.txt,v 1.1 2004/05/01 04:15:38 greear Exp $
