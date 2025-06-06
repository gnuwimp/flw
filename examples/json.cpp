// Copyright gnuwimp@gmail.com
// Released under the GNU General Public License v3.0

#include "flw.h"

int main() {
    // [gnu::json json example]

static const std::string JSON = R"(
{
    "glossary": {
        "title": "example glossary",
        "GlossDiv": {
            "title": "S",
            "GlossList": {
                "GlossEntry": {
                    "ID": "SGML",
                    "SortAs": "SGML",
                    "GlossTerm": "Standard Generalized Markup Language",
                    "Acronym": "SGML",
                    "Abbrev": "ISO 8879:1986",
                    "GlossDef": {
                        "para": "A meta-markup language, used to create markup languages such as DocBook.",
                        "GlossSeeAlso": ["GML", "XML"]
                    },
                    "GlossSee": "markup"
                }
            }
        }
    }
}
)";

    gnu::json::JS            js = gnu::json::decode(JSON);
    const gnu::json::JS*     o  = js.find("GlossEntry", true);
    const gnu::json::JSArray a1 = o->vo_to_va();
    const gnu::json::JS*     a2 = js.find("GlossSeeAlso", true);
    
    puts(o->to_string().c_str());
    
    for (auto v : a1) {
        printf("\t%s: %s => %s\n", o->name_c(), v->name_c(), v->to_string().c_str());
    }
    
    puts("");
    
    for (auto v : *a2->va()) {
        printf("\t%s: %s\n", a2->name_c(), v->vs_c());
    }

/*
GlossEntry: OBJECT: 7 children
	GlossEntry: Abbrev => Abbrev: STRING: ISO 8879:1986
	GlossEntry: Acronym => Acronym: STRING: SGML
	GlossEntry: GlossDef => GlossDef: OBJECT: 2 children
	GlossEntry: GlossSee => GlossSee: STRING: markup
	GlossEntry: GlossTerm => GlossTerm: STRING: Standard Generalized Markup Language
	GlossEntry: ID => ID: STRING: SGML
	GlossEntry: SortAs => SortAs: STRING: SGML

	GlossSeeAlso: GML
	GlossSeeAlso: XML
*/

    // [gnu::json json example]

    // [gnu::json builder example]

    gnu::json::Builder bld;
    
    try {
        bld << bld.MakeObject();
            bld << bld.MakeObject("object");
                bld << bld.MakeNumber(1, "number");
                bld << bld.MakeString("Hello\nWorld", "string");
                bld << bld.MakeBool(true, "bool");
                bld << bld.MakeArray("numbers");
                    bld << bld.MakeNumber(123);
                    bld << bld.MakeNumber(456);
                    bld << bld.MakeNumber(789.123);
                bld.end();
            bld.end();
            
            bld << bld.MakeArray("array");
                bld << bld.MakeNumber(1);
                bld << bld.MakeString("Hello");
                bld << bld.MakeNull();
                bld << bld.MakeBool(false);
            bld.end();

            bld << bld.MakeArrayInline("inline");
                bld << bld.MakeNumber(333);
                bld << bld.MakeNumber(666);
                bld << bld.MakeNumber(999);
            bld.end();
    }
    catch (const std::string& e) {
        puts(e.c_str());
    }

    puts(bld.encode().c_str());

/*
{
	"array": [
		1,
		"Hello",
		null,
		false
	],
	"inline": [333,666,999],
	"object": {
		"bool": true,
		"number": 1,
		"numbers": [
			123,
			456,
			789.123
		],
		"string": "Hello\nWorld"
	}
}
*/

    // [gnu::json builder example]

    return 0;
}
