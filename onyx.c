#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <unixcube/op_types.h>
#include <unixcube/op_fio.h>

#define _onuux_max_line_input_bytes_ 1024

typedef struct onuux_line_t
{
    _c* line_sz;
    _ul line_len;
    _ul index, state;

    struct onuux_line* next;
}onuux_line;

_ul count_exchars(_c* sz, _c exch)
{
    _ul i0;
    _ul exch_i=0;
  
    _ul szlen = strlen((_cc*)sz);
    for(i0=0; i0 <= szlen-1; ++i0){
        if(sz[i0] == exch){
            printf("-%c-",sz[i0]);
            exch_i++;
        }
    }
   szlen -= exch_i;
    printf("exlc[%i]\n", szlen);
   
    return(szlen);
}
/*
_ul strcpy_genlen(_c* sz1, _c excl)
{
    _ul len = strlen((_cc*)sz1)-count_exchars(sz1, excl);
    printf("would copy %i chars\n", len);
    return(len);
}*/

_ul strcpy_exch(_c* sz0, _c* sz1, _c excl)
{
    _ul i0;
    _ul alloc_bytes = count_exchars(sz1, excl);
    sz0 = (_c*)malloc(sizeof(_c) * alloc_bytes);

    for(i0=0; i0 <= alloc_bytes-1; ++i0){
        if(sz1[i0] != excl){
            sz0[i0] = sz1[i0];
        } 
    }
    return(alloc_bytes);
}

onuux_line* new_line_assign(_c* sz, onuux_line* prev)
{
    _ul szlen = strlen((_cc*)sz);
    onuux_line* r = (onuux_line*)malloc(sizeof(onuux_line) );
    r->line_sz = (_c*)malloc((sizeof(_c) * szlen)+1);
    //strcpy_exch(r->line_sz, sz, '~');
    strcpy(r->line_sz, sz);
    r->line_len = szlen;
    r->index=0;
    r->state=1;
    r->next=0;
    if(prev != 0){
        r->index = prev->index+1;
        prev->next = r;
    }
    return(r);
}

onuux_line* new_line_init()
{
    onuux_line* head = (onuux_line*)malloc(sizeof(onuux_line) );
    head->line_sz=0; head->line_len=0; head->index=0; head->state=0;
    head->next=0;
    return(head);
}

onuux_line *parseLine(FILE *file, _c endl_tok)
{
  _ul line_count;
  onuux_line* head = new_line_init();
  onuux_line* util =0;

  char *lineBuffer=calloc(1,1), line[_onuux_max_line_input_bytes_];

  if ( !file || !lineBuffer )
  {
    fprintf(stderr,"an ErrorNo 1: ...");
    exit(1);
  }
  line_count=0;
  for(util=head; fgets(line,sizeof line,file) ; strcat(lineBuffer,line) )
  {
    if( strchr(line, endl_tok) ) *strchr(line, endl_tok)=0;
    //lineBuffer=realloc(lineBuffer, strlen(line)+1);
    if( !line )
    {
      fprintf(stderr,"an ErrorNo 2: ...");
      exit(2);
    }else{
      new_line_assign(line, util);
      util=util->next;
      line_count++;
      printf("entered line[%i] of %i bytes", util->index, util->line_len);
      printf(" :: %s|\n",util->line_sz); 
    }
  }
  printf("\n\nentered %i lines...\n",line_count);
  head->state = line_count;
  return head;
}

_c* encrypt_pwlf_file(_c* fnam, _c* password_master)
{
return(0);
}

_c* get_decrypted_pwlf_data(_c* fnam, _c* password_master)
{
return(0);
}

thc4_n* thc4_multisymmetric_variable_precision_hashvect( onuux_line* pwdb)
{

}

_i main(_i argc, _c** argv)
{
    _i i0;
    FILE* fp=fopen(argv[1], "rw+b");
    onuux_line* line_stack = parseLine(fp, ';');
    if(line_stack->state > 1){
        printf("appartently we got %i lines..\n", line_stack->state);

        for( i0=0; i0 <= 4; ++i0){
            printf("string[%i]::%s\n", line_stack->index, line_stack->line_sz);
            line_stack = line_stack->next;
        }
    }else{
        printf("we aint got shit, ese...\n");
    }
    printf("exiting program..\n");
    return(0);
}