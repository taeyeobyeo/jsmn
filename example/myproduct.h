typedef struct{
	int tokindex;
	int objectList;
} NameTokenInfo;

typedef enum {
	Tamsa = 0,
	Donga = 1,
	CocaCola = 2,
	Lotte = 3,
} company_t;

typedef struct {
	company_t company;
	char name[20];
	int price, count;
	int total;
}product_t;
