#ifndef WF_XML_H
#define WF_XML_H
#include <iostream>
#include <vector>

/*void output(std::string input)
{
	//this is a placeholder for cout, out to file, whatever other out stream I want to use
	std::cout << input;
}*/

class xml_element
{
public:
	xml_element(std::string i_tag) :tag(i_tag), text(""), attribute(""), tab_count(0) {};
	xml_element(std::string i_tag, std::string i_text) :tag(i_tag), text(i_text), attribute(""), tab_count(0) {};
	xml_element(std::string i_tag, std::string i_text, std::string i_attribute) :tag(i_tag), text(i_text), attribute(i_attribute), tab_count(0) {};
	std::string tag;
	std::string text;
	std::string attribute;
	int tab_count;
	std::vector<xml_element> children;

	void add_child(std::string i_tag, std::string i_text = "", std::string i_attribute = "")
	{
		children.push_back(xml_element{ i_tag, i_text, i_attribute });
		children.back().tab_count = tab_count + 1;
	}

	void print(std::ofstream& output)
	{
		for (int i = 0; i < tab_count; i++)
		{
			output << "\t";
		}

		output << "<" << tag << (attribute != "" ? " ":"") << attribute << ">" << text;

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