#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <unixcube/op_types.h>
#include <unixcube/thc4_lib.h>

#define _onuux_max_line_input_bytes_ 1024

typedef struct vec_2d_t { _ul x, y; }vec_2d;
typedef struct vec_3d_t { _ul x, y, z; }vec_3d;

#define _v2D vec_2d
#define _v3D vec_3d

typedef struct onuux_line_t
{
    _c* line_sz;
    _ul line_len;
    _ul index, state;

    struct onuux_line* next;
}onuux_line;

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

  char line[_onuux_max_line_input_bytes_];

  if ( !file  )
  {
    fprintf(stderr,"an ErrorNo 1: ...");
    exit(1);
  }
  line_count=0;
  for(util=head; fgets(line,sizeof line,file) ; )         
  {
    if( strchr(line, endl_tok) ) *strchr(line, endl_tok)=0;
   
    if( !line )
    {
      fprintf(stderr,"an ErrorNo 2: ...");
      exit(2);
    }else{
      new_line_assign(line, util);
      util=util->next;
      line_count++;
     
    }
  }
 
  head->state = line_count;
  return head;
}

#define oFrameNode onuux_line

typedef struct onyx_dataset_t
{
    _c* name;

    oFrameNode* master;

    _ul frame_count;
    _ul longest_frame;
    _ul current_frame;
    
    _ul ds_index;
    _ul ds_state;
    thc4_n* ds_hash;
    struct onyx_dataset_t* next;

}onyx_dataset;

onyx_dataset* newset_assign(_c* setname, onyx_dataset* prev)
{
    onyx_dataset* newset = (onyx_dataset*)malloc(sizeof(onyx_dataset) );
    if(setname){
        newset->name = (_c*)malloc(sizeof(_c) * strlen((_cc*)setname));
        strcpy(newset->name, setname);
    }else{
        newset->name=0;
    }
    newset->master = new_line_init();
    newset->frame_count = prev->frame_count+1;
    newset->longest_frame=0;
    newset->current_frame=0;

    newset->ds_index=newset->frame_count;
    newset->ds_state = 1;
    newset->ds_hash=0;

    newset->next=0;

    if(prev != 0)
        prev->next = newset;

    return(newset);
}

onyx_dataset* newset_init()
{
    onyx_dataset* newset = (onyx_dataset*)malloc(sizeof(onyx_dataset) );
    newset->name=0; newset->master=0; newset->frame_count=0;
    newset->longest_frame=0;newset->current_frame=0;
    newset->ds_index=0;newset->ds_state=0;
    newset->ds_hash=0;newset->next=0;
    return(newset);
}


_i strcmp_avec(_c* sz0, _c* sz1, _ul ind0, _ul ind1)
{
    _ul i0, i1;
    _i is_match;
    _ul lim_a = strlen((_cc*)sz0);
    _ul lim_b = strlen((_cc*)sz1);
    is_match=0;
    for(i0=ind0, i1=ind1; i0 <= lim_a-1 && i1 <= lim_b-1; ++i0, ++i1){
        if(sz0[i0] == sz1[i1]){
            is_match++;
        }else{
            is_match=0;
            break;
        }
    }
    is_match = is_match > 0 ? 1:0;
    return(is_match);
}

_v2D find_in_dataset(onyx_dataset* dset, _c* pattern, _i method)
{
    _v2D v;
    _ul li0, li1;
    _ul len_tmp0, len_tmp1, 
    _ul len_dif_t, len_dif;
    onuux_line* util = dset->master;
        
    for(li0=0, v.x=0, v.y=0; ; ++v.x){
        len_tmp0 = util->line_len;
        len_tmp1 = strlen((_cc*)pattern);
        len_dif_t = len_tmp0 > len_tmp1 ? 0 : 1;
        len_dif_t = len_dif_t ? len_tmp0 - len_tmp1 : 
                                  len_tmp1 - len_tmp0;

        if(len_dif_t){
            for(li0=0; li0 <= len_dif_t-1; ++li0){
                if(util->line_sz[0] == pattern[li0]){
                    v.y = strcmp_avec(util->line_sz, pattern, 0, li0) ? li0 : 0; 
                    if(v.y)
                        break; 
            }   }
        }else{
            for(li0=0; li0 <= len_dif_t-1; ++li0){
                if(pattern[0] == util->line_sz[li0]){
                    v.y = strcmp_avec(pattern, util->line_sz, 0, li0) ? li0 : 0; 
                    if(v.y)
                        break;                                              
        }   }    }
        if(v.y){
            break;
        }else{
            if(util->next != 0)
                util = util->next;
            else
                break;
    }   }
    return(v);
}

_ul oxds_set_name(onyx_dataset* dset, _c* dsname)
{
    dset->name=(_c*)malloc( (sizeof(_c) * strlen((_cc*)dsname))+1);
    strcpy(dset->name, dsname);
    return(strlen((_cc*)dset->name));
}

onyx_dataset* chop_file(_c* fnam, _c delim)
{
    FILE* fp = fopen(fnam, "rw+b");

    onyx_dataset* dset=newset_init();
   
    newset_assign(fnam, dset);

    dset->master = parseLine(fp, delim);

    fclose(fp);
    return(dset);
}


_i main(_i argc, _c** argv)
{
    _i i0;
    onyx_dataset* ds0=0, *ds1=0;
    for(i0=0; i0 <= argc-2; ++i0){
        ds0 = chop_file(argv[i0+1], ';' );
        ds1=ds0;
        ds1=ds1->next;
        printf("chopped file: %s\n", ds1->name);
    }
    return(0);
}
