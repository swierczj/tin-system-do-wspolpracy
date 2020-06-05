#include <vector>
#include <utility>
#include <list>
#include <cstdlib>
#include <algorithm>
#include <string>

#define BUFFER_SPLITTER_ASCII (char)(28)
#define CHAR_SPLITTER_ASCII (char)(29)


// class is rewriten code from java class: Notepad.java
// in order to understand what is inside, please go to Notepad.java
// I didn't put any comments here
class Note{
	struct Character{
		char c;		// coded character
		std::vector< std::pair< int, int > > position;	// position of character
	};
	std::vector< std::string > addedChars;
	std::vector< std::string > deletedChars;
	int _serverId;
	
public:
	Note( int serverId ) : _serverId( serverId ){};
	~Note(){};

private: 
	std::vector< std::string > split( const std::string& str, const std::string& delim, bool empty = true ){
		std::vector< std::string > tokens;
		size_t prev = 0, pos = 0;
		do
		{
			pos = str.find( delim, prev );
			if( pos == std::string::npos ) pos = str.length();
			std::string token = str.substr( prev, pos - prev );
			if( empty || !token.empty() ) 
				tokens.push_back( token );
			prev = pos + delim.length();
		} while( pos < str.length() && prev < str.length() );
		return tokens;
	}

	Character toCharacter( const std::string& str ){
		Character c;
		c.c = str[ 0 ];
		auto splitStr = split( str.substr( 1 ), "_", false );
		for( auto i = splitStr.begin(); i < splitStr.end(); ++i ){
			auto splitPair = split( *i, "," );
			c.position.push_back( std::make_pair( atoi( splitPair[ 0 ].c_str() ), atoi( splitPair[ 1 ].c_str() ) ) );
		}
		return c;
	}

	bool compare( Character first, Character second ){
		for( int i = 0; ; ++i ){
			if( first.position.size <= i ){
				if( second.position.size <= i ){
					if( first.position[ i - 1 ].second < second.position[ i - 1 ].second )
						return false;
					if( first.position[ i - 1 ].second > second.position[ i - 1 ].second )
						return true;
					if( first.c == second.c )
						return false;
					return false;
				}
				return true;
			}
			if( second.position.size <= i )
				return false;
			if( first.position[ i ].first > second.position[ i ].first )
				return false;
			if( second.position[ i ].first > first.position[ i ].first )
				return true;
		}
		return false;
	}

public:
	// Function remembers given changes directly from clients
	void applyChanges( std::string changes ){
		auto buffers = split( changes, std::string( 1, BUFFER_SPLITTER_ASCII ) );
		auto added = split( buffers[ 1 ], std::string( 1, CHAR_SPLITTER_ASCII ), false );
		auto deleted = split( buffers[ 0 ], std::string( 1, CHAR_SPLITTER_ASCII ), false );
		addedChars.insert( std::end( addedChars ), std::begin( added ), std::end( added ) );
		addedChars.insert( std::end( addedChars ), std::begin( added ), std::end( added ) );
	}

	// Function returns whole text as string from all remembered changes
	std::string toString(){
		std::vector< Character > charsToConvert;
		for( auto i = addedChars.begin(); i < addedChars.end(); ++i ){
			if( std::find( deletedChars.begin(), deletedChars.end(), *i ) == deletedChars.end() )
				charsToConvert.push_back( toCharacter( *i ) );
		}
		std::sort( charsToConvert.begin(), charsToConvert.end(), compare );
		std::string converted;
		for( auto i = charsToConvert.begin(); i < charsToConvert.end(); ++i )
			converted += ( *i ).c;
		return converted;
	}

	// Function sets buffers from given text and returns coded messege
	// which can be directly send to clients
	std::string setBuffers( std::string text ){
		std::string changes( 1, BUFFER_SPLITTER_ASCII );
		for( int i = 0; i < text.length; ++i ){
			addedChars.push_back( text[ i ] + std::to_string( i ) + "," + std::to_string( _serverId ) + "_" );
			changes += addedChars[ i ] + std::string( 1, CHAR_SPLITTER_ASCII );
		}
		return changes;
	}

	// Function returns sting with coded file during work
	std::string getFileChanges(){
		std::string changes( 1, BUFFER_SPLITTER_ASCII );
		for( auto i = addedChars.begin(); i < addedChars.end(); ++i ){
			if( std::find( deletedChars.begin(), deletedChars.end(), *i ) == deletedChars.end() )
				changes += *i + std::string( 1, CHAR_SPLITTER_ASCII );
		}
		return changes;
	}
};
