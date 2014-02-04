/*
 * Builds huffman table from frequency of characters.
 */

#include "huff_table.h"

//#define DBG_FRQ
//#define DBG_SORT
//#define DBG_BLD_TREE
//#define DBG_TABLE
//#define GEN_DOT

struct node
{
    unsigned serial;
    char code[256];
    struct node *left; /* The left child of this node. Null if leaf node. */
    struct node *right; /* The right child of this node. Null if leaf node. */
    uint64_t freq; /* The frequency of this node. Combined value if non-leaf node. */
    bool visited; /* Used for searching algorithms. */
    uint8_t c; /* The character this node represents. Null if non-leaf node. */
};

void gen_huff_table(uint64_t freq[256], char *table[256])
{
    node *nodes[256];
    init_nodes(nodes);
    byte_freq(nodes, freq); /* Determine the frequency of occurrence of each byte in the input file. */
    sort_nodes(nodes); /* Sort nodes before building tree. */
    build_tree(nodes);
    create_compression_table(nodes, table);
    
#ifdef GEN_DOT
    tree_dot(*nodes);
#endif
    
    for (int i = 0; i < 511; i++)
        free(all_nodes[i]);
}

void init_node(node **n, char c)
{
    *n = (node*)malloc(sizeof(node));
    for (int i = 0; i < 256; i++)
        (*n)->code[i] = 0;
    (*n)->left = NULL;
    (*n)->right = NULL;
    (*n)->freq = 0;
    (*n)->visited = false;
    (*n)->c = c;
    (*n)->serial = serial++;
}

void init_nodes(node *nodes[256])
{
    for (int i = 0; i < 256; i++)
    {
        init_node(&nodes[i], i);
        all_nodes[all_node_curr++] = nodes[i];
    }
}

void byte_freq(node **nodes, uint64_t freq[256])
{
    for (int i = 0; i < 256; i++)
        nodes[i]->freq = freq[i];
#ifdef DBG_FRQ
    int i;
    for(i = 0; i < 256; i++)
        printf("%d:\t%llu\n", nodes[i]->c, nodes[i]->freq);
#endif
}

void clear_visited()
{
    for (int i = 0; i < 511; i++)
        if (all_nodes[i] != NULL)
            all_nodes[i]->visited = false;
}

char find_lowest(node *root)
{
    clear_visited();
    char lowest = root->c;
    dfs(root, &lowest);
    return lowest;
}

int compare_nodes(const void* node1, const void* node2)
{
    node **n1 = (node**)node1;
    node **n2 = (node**)node2;
    
    /* Null nodes get sorted to end of array. */
    if (*n1 == NULL)
        return 1;
    if (*n2 == NULL)
        return -1;
    
    if ((*n1)->freq == (*n2)->freq)
    {
        char a = find_lowest(*n1);
        char b = find_lowest(*n2);
        return a > b ? -1 : a == b ? 0 : 1;
    }
    
    /* Sort least to greatest. */
    return (*n1)->freq < (*n2)->freq ? -1 : (*n1)->freq == (*n2)->freq ? 0 : 1;
}

void sort_nodes(node **nodes)
{
    dfs_type = SORT;
    qsort(nodes, 256, sizeof(node*), compare_nodes);
    
#ifdef DBG_SORT
    int i;
    for(i = 0; i < 256; i++)
         if (nodes[i] != NULL)
            printf("%c:\t%lld\n", nodes[i]->c, nodes[i]->freq);
 #endif
}

void build_tree(node **nodes)
{
    /* N-1 steps to create tree. */
    for (int i = 0; i < 255; i++)
    {
        /* Remove first two elements. */
        node* node1 = nodes[0];
        node* node2 = nodes[1];
        nodes[0] = NULL;
        nodes[1] = NULL;
        
        /* Combine into new node whose frequency is the sum of frequency of removed nodes. */
        node* new_node;
        init_node(&new_node, 0); /* Character value shouldn't matter here because char comparisons are only done on leaf nodes. */
        new_node->freq = (*node1).freq + (*node2).freq;
        
        /* New node should contain the removed nodes as subtrees. */
        new_node->left = node1;
        new_node->right = node2;
        
        /* Insert new element into list.*/
        nodes[0] = new_node;
        all_nodes[all_node_curr++] = new_node;
        
        /* Sort. */
        sort_nodes(nodes);
    }
    
#ifdef DBG_BLD_TREE
    printf("%d:\t%lld\n", nodes[0]->c, nodes[0]->freq);
    for (int i = 1; i < 256; i++)
        assert(nodes[i] == NULL);
#endif
}

/* Builds a compression table from huffman tree. */
void create_compression_table(node **nodes, char *table[256])
{
    dfs_type = TABLE;
    clear_visited();
    dfs(*nodes, table);
}

/* Preorder depth first search tree traversal from given root node.*/
void dfs(node *root, void *p)
{
    root->visited = true;
    if (root->left != NULL)
    {
        if (root->left->visited == false)
        {
            if (dfs_type == DOT)
                fprintf((FILE *)p, "%d -> %d [label=\"0\"];\n", root->serial, root->left->serial);
            if (dfs_type == TABLE)
                strcat(strcat(root->left->code, root->code), "0");
            dfs(root->left, p);
        }
    }
    if (root->right != NULL)
    {
        if (root->right->visited == false)
        {
            if (dfs_type == DOT)
                fprintf((FILE *)p, "%d -> %d [label=\"1\"];\n", root->serial, root->right->serial);
            if (dfs_type == TABLE)
                strcat(strcat(root->right->code, root->code), "1");
            dfs(root->right, p);
        }
    }
    if(root->right == NULL && root->left == NULL);
    {
        if (dfs_type == SORT && root->c < *(char *)p)
                *(char *)p = root->c;
        if (dfs_type == TABLE)
        {
            ((char **)p)[root->c] = root->code;
#ifdef DBG_TABLE
            printf("%c:\t%llu\t%s\n", root->c, root->freq, root->code);
#endif
        }
    }
}

void tree_dot(node *root)
{
    dfs_type = DOT;
    clear_visited();
    FILE *dot = fopen("tree.dot", "w");
    fputs("digraph G {\n", dot);
    for (int i = 0; i < 511; i++)
    {
        if (i > 31 && i < 127 && i != 34 && all_nodes[i]->freq > 0)
            fprintf(dot, "%u [label=\"'%c' frq: %llu code:%s\"];\n", all_nodes[i]->serial, all_nodes[i]->c, all_nodes[i]->freq, all_nodes[i]->freq > 0 ? all_nodes[i]->code : "");
        else if (i < 256)
            fprintf(dot, "%u [label=\"'\\%u' frq: %llu code:%s\"];\n", all_nodes[i]->serial, all_nodes[i]->c, all_nodes[i]->freq, all_nodes[i]->freq > 0 ? all_nodes[i]->code : "");
        else
            fprintf(dot, "%u [label=\"INNER frq: %llu\"];\n", all_nodes[i]->serial, all_nodes[i]->freq);
    }
    
    dfs(root, dot);
    fputs("}", dot);
    fclose(dot);
}
