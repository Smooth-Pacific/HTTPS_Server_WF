#ifndef WF_XML_H
#define WF_XML_H
#include <iostream>
#include <vector>

/*void output(std::string input)
{
	//this is a placeholder for cout, out to file, whatever other out stream I want to use
	std::cout << input;
}*/

//https://www.w3.org/TR/xml/
//Legal characters are tab, carriage return, line feed, and the legal characters of Unicode and ISO/IEC 10646
//Disallowed initial characters for Names include digits, diacritics, the full stop and the hyphen.



class xml_element
{
	xml_element(std::string i_tag, int i_tab_count, std::string i_text = "", std::string i_attrname = "", std::string i_attribute = "") : is_root(false), attrname(i_attrname), attribute(i_attribute), tab_count(i_tab_count), tag(i_tag), text(i_text) {}
public:
	xml_element(std::string i_tag, std::string i_text = "", std::string i_attrname = "", std::string i_attribute = "") :tag(i_tag), text(i_text), attrname(i_attrname), attribute(i_attribute), is_root(true), tab_count(0) {}
	
	bool is_root;
	std::string tag;
	std::string text;
	std::string attribute;
	std::string attrname;
	int tab_count;
	std::vector<xml_element> children;

	size_t add_child(std::string i_tag, std::string i_text = "", std::string i_attrname = "", std::string i_attribute = "")
	{
		children.push_back(xml_element{ i_tag, (tab_count + 1), i_text, i_attrname, i_attribute});
		//children.back().tab_count = tab_count + 1;
		return children.size() -1;	// -1 to convert from size to index
	}

	void print(std::ofstream& output)
	{
		if (is_root)
		{
			output << "<?xml version=\"1.0\"?>" << std::endl;
		}

		for (int i = 0; i < tab_count; i++)
		{
			output << "\t";
		}

		//output << "<" << tag << (attribute != "" ? " ":"") << attribute << ">" << text;
		
		output << "<" << tag;
		if (attribute != "")
		{
			output << " " << attrname << "=\"" << attribute << "\"";
		}
		output << ">" << text;

		if (children.size() != 0)
		{
			output << "\n";
			for (int i = 0; i < children.size(); i++)
			{
				children[i].print(output);
			}

			for (int i = 0; i < tab_count; i++)
			{
				output << "\t";
			}
		}
		output << "</" << tag << ">\n";
	}

};




/*
	void print(std::ofstream& output)
	{
		for (int i = 0; i < tab_count; i++)
		{
			output("\t");
		}

		output("<" + tag + (attribute != "" ? " ":"") + attribute + ">" + text);

		if (children.size() != 0)
		{
			output("\n");
			for (int i = 0; i < children.size(); i++)
			{
				children[i].print();
			}

			for (int i = 0; i < tab_count; i++)
			{
				output("\t");
			}
		}
		output("</" + tag + ">\n");
	}
*/
#endif