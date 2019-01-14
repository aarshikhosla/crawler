#include<stdio.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
struct node{
  char urL[100];
  int arrKey;
  struct node *next;
  int isVisited;
  int depY;
};
struct node *start=NULL;
#define HASH_SIZE 100

#define URL_LENGTH 1000

#define SEED_URL "www.chitkara.edu.in"

#define MAX_URL_PER_PAGE 1000

#define INTERVAL_TIME 10
#define CHECK_MALLOC(s) if ((s) == NULL) { \
printf("memory allocation failed in %s: line%d", _FILE, __LINE_); \
perror(":"); \
exit(-1); \
}
#define SET_MEM(start,size) memset(start,0,size)
int countFileNum=0;
int SEED_LENGTH=20;

char *urlPointers[100];
struct node* hash[100];
char* tag = "href";

void insertAtEnd(const int k,char* str,int d){
  struct node *n,*ptr=start,*prev=start;
  n=(struct node*)calloc(1,sizeof(struct node)); 
  n->next=NULL;
  strcpy(n->urL,str);
  n->arrKey=k;
  n->isVisited=0;
  n->depY=d;
    if(start==NULL){
        start=n;
      hash[n->arrKey]=start;
    }
    else if(k < start->arrKey){
        n->next=start;
        start=n;
        hash[n->arrKey]=n;
    }
    else{
        while(ptr!=NULL){
          if(ptr->arrKey > k)break;
          prev=ptr;
          ptr=ptr->next;
        }
          if(prev->arrKey!=n->arrKey)hash[n->arrKey]=n;//setting the hash table
            n->next=ptr;
            prev->next=n;
    }
}
char to_lower(char c) {
	return c > 92 ? c : c + 32;
}
char is_whitespace(char c) {
	return c == '\t' || c == '\n' || c == ' ';
}
int url_extract(char *str, int n, char **p) {//gets urls 
	char *t = *p, *c = tag;
	char quote;

	while(*t != '\0') {
		while(*t && *c && to_lower(*t) == *c) {
			t++;
			c++;
		}

		if(*c == '\0') {
			int i = 0;
			while(*t != '\0' && is_whitespace(*t))
				t++;
			if(*t++ == '=') {
				while(*t != '\0' && is_whitespace(*t))
					t++;
				if(*t == '\'' || *t == '\"') {
					quote = *t++;
					while(*t != '\0' && *t != quote && i < n - 1)
				 		str[i++] = *t++;
					str[i] = '\0';
				}
			}
			if(i != 0) break;
		}
		c = tag;
		t++;
	}

	*p = t;
	return *t;
}
void getpage(char *url){//gets page for that url and puts it in temp.txt
  char urlbuffer[URL_LENGTH+300]={0};
  
  strcat(urlbuffer, "wget --no-check-certificate -O ");

  strcat(urlbuffer,"~/Documents/c++/temp.txt ");

  strcat(urlbuffer, url);


  system(urlbuffer);

}
void checkDepth(int d){
    if(d<=1||d>4) {
            printf("Invalid depth ");
            exit(1);
    }
}
void testDir(char *dir){
  struct stat statbuf;
  if ( stat(dir, &statbuf) == -1 )
  {
    fprintf(stderr, "-----------------\n");
    fprintf(stderr, "Invalid directory\n");
    fprintf(stderr, "-----------------\n");
    exit(1);
  }
  //Both check if there's a directory and if it's writable
  if ( !S_ISDIR(statbuf.st_mode) )
  {
    fprintf(stderr, "-----------------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. Your input isn't a directory\n");
    fprintf(stderr, "-----------------------------------------------------\n");
    exit(1);
  }

  if ( (statbuf.st_mode & S_IWUSR) != S_IWUSR )
  {
    fprintf(stderr, "------------------------------------------\n");
    fprintf(stderr, "Invalid directory entry. It isn't writable\n");
    fprintf(stderr, "------------------------------------------\n");
    exit(1);
  }
}
void checks(int d,char *dir){
    checkDepth(d);
    testDir(dir);
}
int checkSize(char *url){
   int i;
    for(i=0;url[i]!='\0';)i++;
    if(i>100)return 0;
    return 1;
}
int checkForSeed(char *url){
    
    char* c;
    c=strstr(url,SEED_URL);
   if(c)return 1;
   return 0;
    
}
int getUrl(){//puts 100 urls in array of pointers
  //char url[2100];
  FILE *fp;//ptr for the count file 
  fp=fopen("temp.txt","r+");

  char *html;//stores complete html file as string to extract urls
  struct stat st;
  stat("temp.txt",&st);
  int file_size=st.st_size;
  html=(char*)malloc(file_size+1);//equals to size of file containing url

  int i=0;
  char c;//transfering to html- the html file
   while((c=fgetc(fp))!=EOF){
    html[i]=c;
    i++;
  }
  fclose(fp);

  html[i]='\0';
	char *t = html; //a temporary variable to hold address accross subsequent calls to url extract
	int count=0;
	while(url_extract(html, 2100, &t)){
  		if(checkSize(html)&&checkForSeed(html)){
        int flag=0;//checking if the url repeats in the array
        for(int i=0;i<count;i++){
          if(strcmp(html,urlPointers[i])==0){
            flag++;
            break;
          }
        }
        if(flag==0){//didn't repeat so insert in array
          urlPointers[count]=(char*)malloc(200);
          strcpy(urlPointers[count],html);
          count++;
        }
      }
      if(count==20)break;
  }
  //put array of pointers in temp.txt
  free(html);
  return count;
}

int generateKey(char *str){
  int i;
  for(i=SEED_LENGTH;str[i]!='/';i++);
  i++;
  int key=0,sum=0;
  for(;str[i]!='/'&&str[i]!='\0'&&str[i]!=' ';i++){
    if(str[i]>='a'&&str[i]<='z')sum=sum+str[i]-'a';
  }
  key=sum;
  while(key>=100){
    key=key%10+key/10;
    key=key/10;
  }
  return key;
}
void insertInLL(int count,int d){//checks hash and inserts in ll
  struct node *ptr;
  int flag=0;
  for(int i=0;i<count;i++){
    int k=generateKey(urlPointers[i]);
    flag=0;
    if(hash[k]!=NULL){
      ptr=hash[k];
      while(ptr!=NULL && ptr->arrKey ==k){
        if(strcmp(ptr->urL,urlPointers[i])==0){
          flag++;
          break;
        }
      ptr=ptr->next;
      }
    }
    if(flag==0)insertAtEnd(k,urlPointers[i],d);
  }
}
int main(int argc, char *argv[]){
    int i;
    char url[URL_LENGTH],dir[100];
    int depth;
    //get url
    for(i=0;argv[1][i]!='\0';i++)url[i]=argv[1][i];
    url[i]='\0';
    //get depth
    depth=(argv[2][0]-48);
    //get directory
    for(i=0;argv[3][i]!='\0';i++)dir[i]=argv[3][i];
    dir[i]='\0';

    checks(depth,dir);
    getpage(url);//get page in templ.txt
    int count=getUrl(1);
    start=NULL;
    //put urls in ll
    for(i=0;i<100;i++)hash[i]=NULL;
    insertInLL(count,depth);//inserting array elements in ll
    
    struct node *ptr=start;
    int k,m=0;
for(ptr=start;ptr!=NULL;ptr=ptr->next)printf("%d %d %d %s\n",m,ptr->depY,ptr->arrKey,ptr->urL);//to print linked list
countFileNum++;

ptr=start;
while(ptr!=NULL){
  if(ptr->isVisited==0){
    if(depth==0)ptr=ptr->next;
    else{
      getpage(ptr->urL);//gets page in temp.txt
      ptr->isVisited=1;
      count=getUrl(1);//gets url count in array of pointers
      insertInLL(count,ptr->depY-1);//puts unique urls in ll
      ptr=start;
    }
  }
else ptr=ptr->next;
}
for(ptr=start;ptr!=NULL;ptr=ptr->next){
  printf("%d %s\n",m,ptr->urL);
  m++;
}

}