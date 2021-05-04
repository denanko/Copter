// Demonstrates using Enhanced GPIO (EGP), the fast way to  
 // do GPIO on certain pins with a PRU.  
 //  
 // Writing to r30 or reading from r31 with PRU0 or PRU1 sets or reads the pins  
 // given in this table:  
 // http://elinux.org/Ti_AM33XX_PRUSSv2#Beaglebone_PRU_connections_and_modes  
 //  
 // But only if the Pinmux Mode has been set correctly with a device  
 // tree overlay!  
 //  
 // Assemble with:  
 // pasm -b pru_egp_io.p  
   
 // Boilerplate  
 .origin 0  
 .entrypoint TOP  
   
 TOP:  
  // Reading bit 14 in the magic PRU GPIO input register 31  
  // bit 14 for PRU0 reads pin 16 on BeagleBone header P8.  
  // If the input bit is high, set the output bit high, and vice versa.  
  QBBS HIGH, r31, 14  
  QBBC LOW, r31, 14  
   
 HIGH:  
  // Writing bit 15 in the magic PRU GPIO output register  
  // register 30, bit 15 for PRU0 turns on pin 11 on BeagleBone  
  // header P8.  
  set r30, r30, 15  
  QBA DONE  
   
 LOW:  
  clr r30, r30, 15  
   
 DONE:  
  // Interrupt the host so it knows we're done  
  mov r31.b0, 19 + 16  
   
 // Don't forget to halt or the PRU will keep executing and probably  
 // require rebooting the system before it'll work again!  
 halt  