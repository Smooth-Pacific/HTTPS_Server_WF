#include "../generate/xml_ele.hpp"

int main()
{

	xml_element test{ "name", "content", "attribute=\"value\"" };
	test.print();



	xml_element test2{ "root" };
	test2.add_child("child1", "content", "attribute = \"value\"");
	test2.add_child("child2", "content2", "attribute = \"value2\"");
	test2.add_child("child3");
	test2.print();



	xml_element test3{ "root" };
	test3.add_child("child1", "content", "attribute = \"value\"");
	test3.add_child("child2", "content2", "attribute = \"value2\"");
	test3.add_child("child3");
	test3.children[2].add_child("sub-child2", "content3", "attribute = \"value3\"");
	test3.children[2].add_child("sub-child2-2", "content3-2", "attribute = \"value3-2\"");

	test3.print();


	return 0;
}