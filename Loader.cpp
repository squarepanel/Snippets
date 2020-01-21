#include <iostream>
#include <vector>
#include <algorithm>
#include <variant>
#include <functional>

using func_ptr_void = std::function<void( void )>;
using func_ptr_str  = std::function<void( const std::string& )>;
using var_func_ptr  = std::variant<func_ptr_void, func_ptr_str>;

template <class... Ts> struct Visitor : Ts... { using Ts::operator()...; };
template <class... Ts> Visitor( Ts... )->Visitor<Ts...>;

struct Loader {
    std::vector<std::pair<std::string, var_func_ptr>> tags;

    void parse( const std::string& line );
};

// tag (:|=) attrubite
void Loader::parse( const std::string& line ) {
    auto b = begin( tags ), e = end( tags );
    auto tag_pos = line.find_first_of( ":=" );
    auto i = std::find_if( b, e, [l = line.substr( 0, tag_pos )]( auto pair ) {
        auto& [tag, func] = pair;
        return l.find( tag, 0 ) != std::string::npos;
    } );
    if( i != e ) {
        std::visit( Visitor { []( func_ptr_void f ) { f(); },
                              [&]( func_ptr_str f ) {
                                  f( line.substr( tag_pos + 1 ) );
                              } },
                    i->second );
    }
}


struct Point {
    int x;
    int y;
};
std::ostream& operator<<( std::ostream& os, const Point& p ) {
    os << '{' << p.x << ',' << p.y << '}';
    return os;
}
int main() {
    std::vector<Point> points;

    Point  p {};
    Loader loader {
        { { "new", [&] { p = Point {}; } },
          { "end", [&] { points.push_back( p ); } },
          { "x", [&]( const std::string& s ) { p.x = std::stoi( s ); } },
          { "y", [&]( const std::string& s ) { p.y = std::stoi( s ); } } }
    };

    auto File = {
        "new object", "x = 3",     "y = 4x",     "end",  " end",       "x = 3",
        " y :  2",    "width : 5", "name : x_w", " end", "new object", " end",
    };
    for( auto& l : File ) loader.parse( l );

    for( auto& p : points ) std::cout << p << '\n';

    return 0;
}
