// Size of kStringTable 135
// Size of kNodeTable 14
// Size of kLeafNodeTable 9
// Total size 223 bytes

static const char kStringTable[] =
"ac\0" "ad\0" "ae\0" "ar\0" "xn--fiqs8s\0" "xn--fiqz9s\0" "xn--kprw13d\0"
"xn--mgbaam7a8h\0" "xn--wgbh1c\0" "ye\0" "yu\0" "za\0" "zm\0" "zw\0" "c"
"om\0" "edu\0" "nom\0" "co\0" "net\0" "!congresodelalengua3\0" "!educ\0"
"*\0" "";

static const struct TrieNode kNodeTable[] = {
  {     0,    14,     2, 1 },  // ac
  {     3,    16,     1, 1 },  // ad
  {     6,    17,     2, 1 },  // ae
  {     9,    19,     3, 0 },  // ar
  {    12,     0,     0, 1 },  // xn--fiqs8s
  {    23,     0,     0, 1 },  // xn--fiqz9s
  {    34,     0,     0, 1 },  // xn--kprw13d
  {    46,     0,     0, 1 },  // xn--mgbaam7a8h
  {    61,     0,     0, 1 },  // xn--wgbh1c
  {    72,    22,     1, 0 },  // ye
  {    75,    22,     1, 0 },  // yu
  {    78,    22,     1, 0 },  // za
  {    81,    22,     1, 0 },  // zm
  {    84,    22,     1, 0 },  // zw
};

static const REGISTRY_U16 kLeafNodeTable[] = {
   87,  // com.ac
   91,  // edu.ac
   95,  // nom.ad
   99,  // co.ae
  102,  // net.ae
  106,  // !congresodelalengua3.ar
  127,  // !educ.ar
  133,  // *.ar
  133,  // *.ye
};

static const size_t kLeafChildOffset = 14;
static const size_t kNumRootChildren = 14;
