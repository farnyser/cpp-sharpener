#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

std::string read(std::istream& in)
{
	std::string str;
	std::stringstream ss;

	while(std::getline(in, str)) 
		ss << str << std::endl;

	return ss.str();
}

enum class Type 
{
	Empty,
	Space,
	Identifier,
	Instruction,
	Literal,
	IdentifierList,
	Block,
	// Comment,
	// Numeric,
	Arrow,
	Unknown
};

struct Token 
{
	Type type;
	std::string text;
	std::string innerText;
	
	static Token Empty() 
	{
		static Token empty;
		empty.type = Type::Empty;
		return empty;
	}
	
	bool operator==(const Token& other) {
		return other.type == type && other.text == text;
	}
	
	bool operator!=(const Token& other) {
		return other.type != type || other.text != text;
	}
};

struct Parser 
{
	size_t position { 0 };
	std::string buffer;
	
	void consume(const std::string& str) { position += str.length(); }
	void consume(const Token& t) { position += t.text.length(); }

	Parser next(const Token& t) 
	{
		for(auto i = position+t.text.length(); i < buffer.length() ; i++) 
			if(buffer[i] != ' ' && buffer[i] != '\t')
				return Parser{i, buffer}; 
				
		return Parser{buffer.length(), buffer};
	}
	
	Token parse(bool parseInstruction = true) 
	{
		while(!eof())
		{
			if(parseInstruction && instruction() != Token::Empty()) 
				return instruction();
			if(literal() != Token::Empty()) 
				return literal();
			if(identifierList() != Token::Empty()) 
				return identifierList();
			if(identifier() != Token::Empty()) 
				return identifier();
			if(arrow() != Token::Empty()) 
				return arrow();
			if(block() != Token::Empty()) 
				return block();
			
			auto str = std::string() + buffer[position];
			return Token { (str == " " || str == "\t") ? Type::Space : Type::Unknown, str, str };
		}
		
		return Token::Empty();
	}

	Token arrow() 
	{
		if(position+1 < buffer.length() && buffer[position] == '=' && buffer[position+1] == '>')
			return Token{Type::Arrow, "=>", "=>"};
		
			return Token::Empty();
	}

	Token identifier() 
	{
		std::string result;	

		for(int i = position; i < buffer.size() ; i++)
		{
			auto x = buffer[i];
			if(x >= '0' && x <= '9')
			{
				if(i == position)
					break;

				result += x;
			}
			else if((x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z') || x == '_')
				result += x;
			else 
				break;
		}

		if(result.length() > 0) 
			return Token { Type::Identifier, result, result };
		
		return Token::Empty();
	}
	
	Token identifierList() 
	{
		std::string result;	
		bool success = false;

		if(!eof() && buffer[position] == '(')
		{
			auto n = Parser{position+1, buffer};
			bool looking_for_comma = false;
			
			while(!n.eof() && !success) 
			{
				auto t = n.parse(false);
				if(t.type == Type::Space)
					;
				else if((looking_for_comma || result.empty()) && t.type == Type::Unknown && t.text == ")")
					success = true;
				else if(looking_for_comma  && t.type == Type::Unknown && t.text == ",")
					looking_for_comma = false;
				else if(!looking_for_comma && t.type == Type::Identifier)
					looking_for_comma = true;
				else 
					return Token::Empty();
				
				if(!success)
					result += t.text;
				
				n.consume(t);
			}
		}
		
		if(success)
			return Token { Type::IdentifierList, "(" + result + ")", result };
		
		return Token::Empty();
	}

	Token literal()
	{
		std::string result;

		char start = 0;
		bool backslash = false;
		for(int i = position, c = 0; i < buffer.size() ; i++) 
		{
			auto x = buffer[i];
			if(i == position && x != '\'' && x != '"')
				break;
			else if(i == position)
				start = x;
			else if(backslash) 
				backslash = false;
			else if(x == '\\') 
				backslash = true;
			else if(x == start)
			{	result += x; break; }

			result += x;
		}

		if(result.length() > 0) 
			return Token { Type::Literal, result, result };
		
		return Token::Empty();
	}

	Token block() 
	{
		std::string result;

		if(!eof() && buffer[position] == '{')
		{
			Parser tmp{position+1, buffer};
			while(true) {
				auto token = tmp.parse(false);
				if(token.type == Type::Unknown && token.text == "}") {
					break;
				}
				result += token.text;
				tmp.consume(token);
			}
		}

		if(result.length() > 0)
			return Token{ Type::Block, "{"+result+"}", result };
		
		return Token::Empty();
	}
	
	Token instruction() 
	{
		std::string result;
		size_t token_count = 0;
		
		int lp = 0;
		auto n = *this;
		while(true) 
		{
			auto t = n.parse(false);		
			if(t.type == Type::Block || n.eof()) 
				break;
			if(t.type == Type::Unknown && t.text == "(") 
				lp++;
			else if(t.type == Type::Unknown && t.text == ")") 
				lp--;
			else if(t.type == Type::Unknown && t.text == ";" ) 
				break;
			else if(token_count == 0 && t.type != Type::Identifier)
				break;
			
			if(lp < 0) 
				break;
			
			result += t.text;
			token_count++;

			n.consume(t);
		}

		if(token_count > 1 && result.length() > 0)
			return Token{ Type::Instruction, result, result };
		
		return Token::Empty();
	}
	
	bool eof() { return position >= buffer.length(); }
};

std::string transform(const std::string& input);

std::string tbegin(const Token& token)
{
	if(token.type == Type::IdentifierList)
	{
		if(token.innerText.empty())
			return "[&](" + token.innerText + ")";

		std::string result = "auto ";
		for(auto& x : token.innerText) 
		{
			if(x != ' ' && x != '\t')
				result += x;
			if(x == ',')
				result += " auto ";
		}
			
		return "[&](" + result + ")";
	}
	
	return "[&](auto " + token.text + ")";
}

std::string tend(const Token& token)
{
	if(token.type == Type::Block)
		return transform(token.text);
		
	return "{ return " + transform(token.text) + "; }";
}

std::string transform(const std::string& input)
{
	Parser buffer{0, input};
	std::string output;
	
	while(!buffer.eof())
	{
		auto token = buffer.parse(false);
		auto next_buffer = buffer.next(token);
		auto next = next_buffer.parse(false);
		auto next_buffer2 = next_buffer.next(next);
		auto next2 = next_buffer2.parse();
		
		if(next.type == Type::Arrow && (token.type == Type::Identifier || token.type == Type::IdentifierList)) {
			output += tbegin(token) + tend(next2);
			buffer = next_buffer2;
			buffer.consume(next2);
			continue;
		}
		
		if(token.type == Type::Block) {
			output += "{" + transform(token.innerText) + "}";
		}
		else {
			output += token.text;
		}
		
		buffer.consume(token);
	}

	return output;
}

int check(const std::string& test, const std::string& expected)
{
	if(expected != test)
	{
		std::cerr << "Expected: " << std::endl
			<< "\t" << expected << std::endl
			<< "But was " << std::endl
			<< "\t" << test << std::endl;
		
		return 1;
	}
	
	return 0;
}

void run_tests()
{
	int error = 0;
	error += check(transform("hello"), "hello");
	error += check(transform("x => x * 2"), "[&](auto x){ return x * 2; }");
	error += check(transform("(x, y) => x+y"), "[&](auto x, auto y){ return x+y; }");
	error += check(transform("(x, y) => (x+y) * 2"), "[&](auto x, auto y){ return (x+y) * 2; }");
	error += check(transform("() => foobar()"), "[&](){ return foobar(); }");
	error += check(transform("(x, y) => { foobar(x); foobar(y); }"), "[&](auto x, auto y){ foobar(x); foobar(y); }");
	error += check(transform("(x => x * x)(2)"), "([&](auto x){ return x * x; })(2)");
	
	if(error != 0)
		throw std::runtime_error("Integrated tests failing!");
}

int main(int argc, char **argv)
{
	run_tests();
	std::cout << transform(read(std::cin));
	return EXIT_SUCCESS;
}