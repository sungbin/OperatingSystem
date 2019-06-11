#include <stdio.h>
#include "smalloc.h"

int 
main()
{
	void *p1, *p2, *p3;

	print_sm_containers() ;

	p1 = smalloc(3000) ; 
	printf("smalloc(3000)\n") ; 
	print_sm_containers() ;

	p2 = smalloc(2500) ; 
	printf("smalloc(2500)\n") ; 
	print_sm_containers() ;

	sfree(p1) ; 
	printf("sfree(%p)\n", p1) ; 
	print_sm_containers() ;

	sfree(p2) ; 
	printf("sfree(%p)\n", p2) ; 
	print_sm_containers() ;

	p3 = smalloc(2200) ; 
	printf("smalloc(2200)\n") ; 
	print_sm_containers() ;

	print_sm_uses();
}
