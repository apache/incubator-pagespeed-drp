The domain registry provider is a space and runtime efficient domain registry lookup library written in C. The library provides a simple API:


`int len = GetRegistryLength("www.example.com"); // Returns 3 since "com" is the domain suffix.`

`len = GetRegistryLength("www.example.co.uk"); // Returns 5 since "co.uk" is the domain suffix.`

`len = GetRegistryLength("www.example.com.au"); // Returns 6 since "com.au" is the domain suffix.`

The list of valid domain suffixes is provided by http://publicsuffix.org/.

The library uses ~30kB for the data tables and ~10kB for the C code. There is no startup overhead since the tables are constructed at compile time. See DesignDoc for more details.