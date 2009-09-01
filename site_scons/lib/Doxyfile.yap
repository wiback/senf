
INPUT                  = .
RECURSIVE              = NO
PROJECT_NAME           = "${PROJECTNAME}"
PROJECT_NUMBER         = "(Version ${REVISION})"

###########################################################################
# Layout customization

DOXYFILE_ENCODING      = UTF-8
INPUT_ENCODING         = UTF-8

ALWAYS_DETAILED_SEC    = YES
MULTILINE_CPP_IS_BRIEF = YES
DETAILS_AT_TOP         = YES

ALIASES                = "fixme=\xrefitem fixme \"Fix\" \"Fixmes\"" \
                         "idea=\xrefitem idea \"Idea\" \"Ideas\"" \
                         "implementation=\par \"Implementation note:\"" \
                         "doc=\xrefitem doc \"Documentation request\" \"Documentation Requests\"" \
                         "autotoc=\htmlonly <div id=\"autotoc\"></div> \endhtmlonly" \
                         "seechapter=<b>\htmlonly &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&rarr;&nbsp;\endhtmlonly see </b>"

BUILTIN_STL_SUPPORT    = YES

SUBGROUPING            = NO
EXTRACT_ALL            = YES
HIDE_FRIEND_COMPOUNDS  = YES
INTERNAL_DOCS          = YES

FILE_PATTERNS          = *.c \
                         *.cc \
                         *.cci \
                         *.ct \
                         *.cti \
                         *.h \
                         *.hh \
                         *.ih \
                         *.mpp \
                         *.dox
EXCLUDE_PATTERNS       = *.test.cc \
                         *.test.hh \
                         .* \
                         *~ \
                         "#*#"
EXCLUDE_SYMBOLS        = boost std
IMAGE_PATH             = .

SOURCE_BROWSER         = YES
ALPHABETICAL_INDEX     = YES
COLS_IN_ALPHA_INDEX    = 3

GENERATE_LATEX         = NO
GENERATE_XML           = NO
GENERATE_MAN           = NO

MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = YES
PREDEFINED             = DOXYGEN \
                         "SENF_PPI_MODULE(x)=" \
                         "SENF_PARSER()=" \
                         "SENF_FIXED_PARSER()=" \
                         "SENF_PARSER_INHERIT(name)=" \
                         "SENF_PARSER_FIELD(name,type)=type name() const" \
                         "SENF_PARSER_FIELD_RO(name,type)=type::value_type name() const" \
                         "SENF_PARSER_BITFIELD(name, bits, type)=senf::ParseField_ ## type(bits) name() const" \
                         "SENF_PARSER_BITFIELD_RO(name, bits, type)=senf::ParseField_ ## type(bits)::value_type name() const" \
                         "SENF_PARSER_CUSTOM_FIELD(name, type, size, isize)=type name() const" \
                         "SENF_PARSER_PRIVATE_FIELD(name,type)=private: type name() const; public:" \
                         "SENF_PARSER_PRIVATE_FIELD_RO(name, type)=private: type::value_type name() const; public:" \
                         "SENF_PARSER_PRIVATE_BITFIELD(name, bits, type)=private: senf::ParseField_ ## type(bits) name() const; public:" \
                         "SENF_PARSER_PRIVATE_BITFIELD_RO(name, bits, type)=private: senf::ParseField_ ## type(bits)::value_type name() const; public:" \
                         "SENF_PARSER_SKIP(x)=" \
                         "SENF_PARSER_SKIP_BITS(x)=" \
                         "SENF_PARSER_GOTO(x)=" \
                         "SENF_PARSER_GOTO_OFFSET(x)=" \
                         "SENF_PARSER_LABEL(x)=" \
                         "SENF_PARSER_INIT()=void init()" \
                         "SENF_PARSER_FINALIZE(name)=" \
                         "ParseField_unsigned(b)=UIntFieldParser<?,?+b>" \
                         "ParseField_signed(b)=IntFieldParser<?,?+b>" \
                         "ParseField_bool(b)=FlagParser<?>" \
                         "SENF_PARSER_ARRAY(name,elt_type,size)=senf::Parse_Array<size,elt_type> name() const" \
                         "SENF_PARSER_LIST_B(name,elt_type,size_type)=senf::Parse_ListB<elt_type,size_type>::parser name() const" \
                         "SENF_PARSER_LIST_N(name,elt_type,size_type)=senf::Parse_ListN<elt_type,size_type>::parser name() const" \
                         "SENF_PARSER_VARIANT(name,chooser,types)=senf::Parse_Variant_Direct<chooser ## _t,?,types>::parser name() const" \
                         "SENF_PARSER_PRIVATE_VARIANT(name,chooser,types)=private: senf::Parse_Variant_Direct<chooser ## _t,?,types>::parser name() const; public:" \
                         "SENF_PARSER_VEC_N(name,elt_type,size_type)=senf::Parse_VectorN<elt_type,size_type> name() const" \
                         "SENF_LOG_CLASS_AREA()=" \
                         "SENF_LOG_DEFAULT_AREA(area)=" \
                         "SENF_LOG_DEFAULT_STREAM(stream)=" \
                         "BOOST_PP_ITERATE()=" \
                         "BOOST_PARAMETER_KEYWORD(ns, name)=unspecified_keyword_type name;"
EXPAND_AS_DEFINED      = prefix_ \
                         SENF_LOG_DEF_STREAM \
                         SENF_LOG_DEF_AREA \
                         SENF_LOG_DEF_AREA_I

EXTERNAL_GROUPS        = NO

HAVE_DOT               = YES
COLLABORATION_GRAPH    = NO
GROUP_GRAPHS           = NO
GRAPHICAL_HIERARCHY    = NO
DIRECTORY_GRAPH        = NO
DOT_GRAPH_MAX_NODES    = 10
MAX_DOT_GRAPH_DEPTH    = 5
DOT_MULTI_TARGETS      = YES
DOT_CLEANUP            = NO

SEARCHENGINE           = YES

###########################################################################
# The following options are MANDATORY to integrate with the build system

OUTPUT_DIRECTORY       = $(output_dir)
STRIP_FROM_INC_PATH    = $(TOPDIR)
LAYOUT_FILE            = $(LIBDIR)/DoxygenLayout.xml
GENERATE_HTML          = $(html)
HTML_OUTPUT            = $(html_dir)
HTML_HEADER            = $(TOPDIR)/$(output_dir)/doxyheader.html
HTML_FOOTER            = $(TOPDIR)/$(output_dir)/doxyfooter.html
INPUT_FILTER           = $(LIBDIR)/filter.pl
TAGFILES               = $(tagfiles)
GENERATE_TAGFILE       = $(generate_tagfile)
DOT_PATH               = $(LIBDIR)

