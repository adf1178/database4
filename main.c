#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "extmem.h"

#define MAX 1000

typedef struct Relation
{
    int fir_data;
    int sec_data;
    int blk;
} Relation;

typedef struct Index
{
    int attribute; //属性
    int blk;       //块
} Index;           //索引

Relation **R;
Relation **S;
int same_number = 0;

int Change(unsigned char *blk, int i, int num)
{
    char str[5];
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + i * 8 + k + num);
    }
    return atoi(str);
}

//
//读数据输出到屏幕
//
int Read(unsigned char *blk, int num, Buffer *buf)
{
    char str[5];
    int X, Y, i;
    if ((blk = readBlockFromDisk(num, buf)) == NULL)
    {
        perror("Reading Block Failed!\n");
        return -1;
    }
    printf("== ");
    for (i = 0; i < 7; i++) //一个blk存7个元组加一个地址
    {
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i * 8 + k);
        }
        X = atoi(str);
        for (int k = 0; k < 4; k++)
        {
            str[k] = *(blk + i * 8 + 4 + k);
        }
        Y = atoi(str);
        printf("(%d, %d) ", X, Y);
    }
    for (int k = 0; k < 4; k++)
    {
        str[k] = *(blk + i * 8 + k);
    }
    int addr = atoi(str);
    printf("==");
    printf("\nnext address = %d \n", addr);
    freeBlockInBuffer(blk, buf);
}

//
//载入数据到指针数组
//
Relation *ReadData(Buffer *buf, int choose)
{
    Relation *temp;
    unsigned char *blk;
    unsigned char temp1[5], temp2[5];
    int number = 0, num, i, j, count;
    if (choose == 0)
    {
        num = 17;
        count = 0;
        temp = (Relation *)malloc(sizeof(Relation) * 112);
    }
    if (choose == 1)
    {
        num = 33;
        count = 16;
        temp = (Relation *)malloc(sizeof(Relation) * 224);
    }
    for (i = 1; i < num; i++)
    {
        if ((blk = readBlockFromDisk(i + count, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return -1;
        }
        for (j = 0; j < 7; j++)
        {
            memcpy(temp1, blk + j * 8, 4);
            memcpy(temp2, blk + j * 8 + 4, 4);
            temp[number].fir_data = atoi((char *)temp1);
            temp[number].sec_data = atoi((char *)temp2);
            temp[number].blk = i + count + 1;
            number++;
        }
        freeBlockInBuffer(blk, buf);
    }
    return temp;
}

//
//数据空位填NULL
//
void Fill_in(unsigned char *string, int number)
{
    char *zero = (char *)malloc(4 * sizeof(char));
    sprintf(zero, "");
    for (int i = number; i < 7; i++)
    {
        memcpy((string + 8 * i), zero, 5);
        memcpy((string + 8 * i + 4), zero, 5);
    }
}

//
//插入排序
//
Relation *insertion_sort(Relation *temp, int len)
{
    int i, j, k;
    Relation V;
    for (i = 1; i < len; i++)
    {
        for (j = i - 1; j >= 0; j--)
            if (temp[j].fir_data < temp[i].fir_data)
                break;
        if (j != i - 1)
        {
            V = temp[i];
            for (k = i - 1; k > j; k--)
                temp[k + 1] = temp[k];
            temp[k + 1] = V;
        }
    }
    return temp;
}

//
//二分搜索
//
void Binary_Search(Buffer *buf, Relation *temp, int choose)
{
    unsigned char *blk = getNewBlockInBuffer(buf);
    char temp1[5], temp2[5];
    char *next = (char *)malloc(4 * sizeof(char));

    int number = 0, address, n, value, write_flag = 0;
    if (choose == 0)
    {
        address = 200;
        n = 112;
        value = 40;
    }
    else if (choose == 1)
    {
        address = 250;
        n = 224;
        value = 60;
    }

    int pos = -1;
    int low = 0, high = n - 1, mid;
    // 二分查找
    while (low < high)
    {
        if (temp[low].fir_data == value || temp[high].fir_data == value)
        {
            if (temp[low].fir_data == value)
            {
                pos = low;
                low++;
            }
            else if (temp[high].fir_data == value)
            {
                pos = high;
                high--;
            }
            write_flag = 0;
            printf("(%d, %d)\n", temp[pos].fir_data, temp[pos].sec_data);
            sprintf(temp1, "%u", temp[pos].fir_data);
            sprintf(temp2, "%u", temp[pos].sec_data);
            memcpy((blk + 8 * number), temp1, 4);
            memcpy((blk + 8 * number + 4), temp2, 4);
            number++;
            Fill_in(blk, number);
            if (number == 7)
            {
                write_flag = 1;
                sprintf(next, "%u", address + 1);
                memcpy((blk + 8 * number), next, 4);
                writeBlockToDisk(blk, address, buf);
                printf("储存在%d.blk\n", address);
                address++;
                blk = getNewBlockInBuffer(buf);
                number = 0;
            }
        }
        mid = low + (high - low) / 2;

        if (temp[mid].fir_data == value)
        {
            pos = mid;
            printf("(%d, %d)\n", temp[pos].fir_data, temp[pos].sec_data);
            sprintf(temp1, "%u", temp[pos].fir_data);
            sprintf(temp2, "%u", temp[pos].sec_data);
            memcpy((blk + 8 * number), temp1, 4);
            memcpy((blk + 8 * number + 4), temp2, 4);
            write_flag = 0;
            number++;
            Fill_in(blk, number);
            if (number == 7)
            {
                write_flag = 1;
                sprintf(next, "%u", address + 1);
                memcpy((blk + 8 * number), next, 4);
                writeBlockToDisk(blk, address, buf);
                printf("储存在%d.blk\n", address);
                address++;
                blk = getNewBlockInBuffer(buf);
                number = 0;
            }
        }

        if (temp[mid].fir_data < value)
            low = mid + 1;
        else
            high = mid - 1;
    }
    if (pos == -1)
        printf("无匹配结果!\n");
    else
    {
        if (write_flag == 0)
        {
            char *Nul = (char *)malloc(1 * sizeof(char));
            sprintf(Nul, "");
            for (int m = 8 * number; m < 64; m++)
                memcpy((blk + m), Nul, 1);
            writeBlockToDisk(blk, address, buf);
            printf("储存在%d.blk\n", address);
        }
    }
    freeBlockInBuffer(blk, buf);
    printf("Number of IO： %d\n", (int)buf->numIO); /* Check the number of IO's */
}

//
//线性搜索
//
void Line_search(Buffer *buf, int choose)
{
    int last = (int)buf->numIO;
    int i, j, A, B, number = 0, address, num, value, flag = 0, count, write_flag = 0;
    unsigned char *Disk, *blk;
    char temp1[5], temp2[5];
    //关系R
    if (choose == 0)
    {
        address = 100;
        num = 17;
        value = 30;
        count = 0;
    } //关系S
    else if (choose == 1)
    {
        address = 150;
        num = 33;
        value = 23;
        count = 16;
    }
    else
        printf("ERROR!");
    Disk = getNewBlockInBuffer(buf);
    for (i = 1; i < num; i++)
    {
        blk = readBlockFromDisk(i + count, buf);
        for (j = 0; j < 7; j++)
        {
            if (Change(blk, j, 0) == value)
            {
                flag = 1;
                write_flag = 0;
                A = value;
                B = Change(blk, j, 4);
                printf("(%d, %d)\n", A, B);
                sprintf(temp1, "%u", A);
                sprintf(temp2, "%u", B);
                memcpy((Disk + 8 * number), temp1, 4);
                memcpy((Disk + 8 * number + 4), temp2, 4);
                number++;
                if (number == 7)
                {
                    write_flag = 1;
                    sprintf(temp1, "%u", address + 1);
                    memcpy((Disk + number * 8), temp1, 4);
                    writeBlockToDisk(Disk, address, buf);
                    printf("储存在%d.blk\n", address);
                    number = 0;
                    Disk = getNewBlockInBuffer(buf);
                    address++;
                }
            }
        }
        freeBlockInBuffer(blk, buf);
    }
    //未填满的部分填NULL
    if (write_flag == 0)
    {
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int i = 8 * number; i < 64; i++)
            memcpy((Disk + i), Nul, 1);
        writeBlockToDisk(Disk, address, buf);
        printf("储存在%d.blk\n", address);
    }
    if (flag == 0)
        printf("无匹配结果!\n");
    printf("Number of IO： %d\n", (int)buf->numIO - last); /* Check the number of IO's */
}

//
//索引查找
//
Index *CreateIndex(Buffer *buf, Relation *temp, int address, int len, int value, int *num)
{
    Index *temp_index = (Index *)malloc(sizeof(Index) * value);
    //    char temp1[5],temp2[5],temp3[5];

    int index_num = *num, i, Count = 0;
    temp_index[index_num].attribute = temp[0].fir_data;
    temp_index[index_num].blk = address;
    unsigned char *blk = getNewBlockInBuffer(buf);
    for (i = 0; i < len; i++)
    {
        char *temp1 = (char *)malloc(4 * sizeof(char));
        char *temp2 = (char *)malloc(4 * sizeof(char));
        char *temp3 = (char *)malloc(4 * sizeof(char));
        sprintf(temp1, "%d", temp[i].fir_data);
        sprintf(temp2, "%d", temp[i].sec_data);
        memcpy((blk + 8 * Count), temp1, 4);
        memcpy((blk + 8 * Count + 4), temp2, 4);
        Count++;
        if ((i > 0) && (temp[i].fir_data > temp[i - 1].fir_data))
        {
            index_num++;
            temp_index[index_num].attribute = temp[i].fir_data;
            // if(Count == 0)  temp_index[index_num].blk = address - 1;
            // else  temp_index[index_num].blk = address;
            temp_index[index_num].blk = address;
        }
        if (Count == 7) // 如果count=7 那么把下一块地址写进去
        {
            if (i == len - 1)
            {
                sprintf(temp3, "%d", 0);
                memcpy((blk + 8 * Count + 4), temp3, 4);
            }
            else
            {
                sprintf(temp3, "%d", (address + 1));
                memcpy((blk + 8 * Count + 4), temp3, 4);
            }
            writeBlockToDisk(blk, address, buf);
            address++;
            Count = 0;
        }
    }
    *num = index_num;
    return temp_index;
}

void Index_search(Buffer *buf, Relation *temp, int choose)
{

    unsigned char *blk, *Disk;
    unsigned char temp1[5], temp2[5], temp3[5];
    int i, j, block = 0, address, value, count = 0, len, number = 0, address_data, flag = 0, write_flag = 0;
    if (choose == 0)
    {
        address = 300;
        value = 30;
        len = 112;
        address_data = 340;
    }
    else if (choose == 1)
    {
        address = 350;
        value = 23;
        len = 224;
        address_data = 390;
    }
    else
        printf("ERROR!");
    Index *index = CreateIndex(buf, temp, address, len, value, &count);
    int last = (int)buf->numIO;
    // for (i=0;i<30;i++)
    //     printf("%d    %d\n",index[i].attribute,index[i].blk);
    Disk = getNewBlockInBuffer(buf);
    for (i = 0; i <= count; i++)
    {
        if (index[i].attribute >= value - 1 && index[i].attribute <= value + 1 && block != index[i].blk) // 第26个块里属性=30
        {
            block = index[i].blk;
            printf("=======Reading block %d\n", block);
            blk = readBlockFromDisk(block, buf);
            for (j = 0; j <= 7; j++)
            {
                unsigned *check = blk + 8 * j;
                if (atoi((char *)check) == value)
                {
                    write_flag = 0;
                    memcpy(temp1, blk + 8 * j, 4);
                    memcpy(temp2, blk + 8 * j + 4, 4);
                    printf("(%d, %d)\n", atoi(temp1), atoi(temp2));

                    flag = 1;
                    memcpy((Disk + 8 * number), temp1, 4);
                    memcpy((Disk + 8 * number + 4), temp2, 4);
                    number++;
                    Fill_in(Disk, number);
                    if (number == 7)
                    {
                        write_flag = 1;
                        sprintf(temp3, "%u", address_data + 1);
                        memcpy((Disk + 8 * number), temp3, 4);
                        writeBlockToDisk(Disk, address_data, buf);
                        printf("储存在%d.blk\n", address_data);
                        address_data++;
                        Disk = getNewBlockInBuffer(buf);
                        number = 0;
                    }
                }
            }
            printf("%d    %d\n", i, count);
            freeBlockInBuffer(blk, buf);
            if ((i == count) && (blk = readBlockFromDisk(block + 1, buf)) != NULL)
            {
                for (j = 0; j < 7; j++)
                {
                    unsigned *check = blk + 8 * j;
                    printf(" check %s  \n", check);
                    if (atoi((char *)check) == value)
                    {
                        memcpy(temp1, blk + 8 * j, 4);
                        memcpy(temp2, blk + 8 * j + 4, 4);
                        printf("(%d, %d)\n", atoi(temp1), atoi(temp2));

                        flag = 1;
                        memcpy((Disk + 8 * number), temp1, 4);
                        memcpy((Disk + 8 * number + 4), temp2, 4);
                        number++;
                        Fill_in(Disk, number);
                        if (number == 7)
                        {
                            sprintf(temp3, "%u", address_data + 1);
                            memcpy((Disk + 8 * number), temp3, 4);
                            writeBlockToDisk(Disk, address_data, buf);
                            printf("储存在%d.blk\n", address_data);
                            address_data++;
                            Disk = getNewBlockInBuffer(buf);
                            number = 0;
                        }
                    }
                }
                freeBlockInBuffer(blk, buf);
            }
        }
    }
    // 如果没有找到
    if (flag == 0)
        printf("无匹配结果!\n");
    else
    {
        if (write_flag == 0)
        {
            char *Nul = (char *)malloc(1 * sizeof(char));
            sprintf(Nul, "");
            for (int m = 8 * number; m < 64; m++)
                memcpy((Disk + m), Nul, 1);
            writeBlockToDisk(Disk, address_data, buf);
            printf("储存在%d.blk\n", address_data);
        }
    }
    freeBlockInBuffer(Disk, buf);
    printf("Number of IO： %d\n", (int)buf->numIO - last); /* Check the number of IO's */
}

//
//选择菜单
//
void SELECT(Buffer *buf, Relation *tempR, Relation *tempS)
{
    int choose = -1;
    while (1)
    {
        printf("\n=======================\n");
        printf("请选择关系选择算法:\n");
        printf("  1.线性搜索算法\n");
        printf("  2.二元搜索算法\n");
        printf("  3.索引搜索算法\n");
        printf("  0.返回主菜单\n");
        printf("=======================\n");
        scanf("%d", &choose);
        switch (choose)
        {
        case 1:
            printf("线性搜索，结果如下;\n------------\nR.A:\n");
            Line_search(buf, 0);
            printf("------------\nS.C:\n");
            Line_search(buf, 1);
            printf("------------\n");
            break;
        case 2:
            printf("二分搜索，结果如下;\n------------\nR.A:\n");
            Binary_Search(buf, tempR, 0);
            printf("------------\nS.C:\n");
            Binary_Search(buf, tempS, 1);
            printf("------------\n");
            break;
        case 3:
            printf("索引搜索，结果如下;\n------------\nR.A:\n");
            Index_search(buf, tempR, 0);
            printf("------------\nS.C:\n");
            Index_search(buf, tempS, 1);
            printf("------------\n");
            break;
        case 0:
            main();
            break;
        default:
            printf("请输入0-3选择功能!");
            break;
        }
    }
}

//
//投影
//
void Projection(Buffer *buf, Relation *temp)
{
    int number = 0, address = 400, i, j, count = 0;
    unsigned char *blk, *Disk;
    char *temp1 = (char *)malloc(4 * sizeof(char));
    char *temp2 = (char *)malloc(4 * sizeof(char));
    for (i = 1; i < 17; i++)
    {
        if ((blk = readBlockFromDisk(i, buf)) == NULL)
        {
            perror("Reading Block Failed!\n");
            return;
        }
        for (j = 0; j < 7; j++)
        {
            number++;
        }
        freeBlockInBuffer(blk, buf);
    }
    blk = getNewBlockInBuffer(buf);
    sprintf(temp1, "%u", temp[0].fir_data);
    memcpy((blk + 4 * count), temp1, 4);
    printf("属性A:\n %s", temp1);
    for (i = 1; i < number; i++)
    {
        if (temp[i].fir_data > temp[i - 1].fir_data)
        {
            count++;
            if (count == 14)
            {
                sprintf(temp2, "%u", address + 1);
                memcpy((blk + 4 * count), temp2, 4);
                writeBlockToDisk(blk, address, buf);
                printf("\n*储存在：%d.blk\n", address);
                address++;
                blk = getNewBlockInBuffer(buf);
                count = 0;
            }
            sprintf(temp1, "%u", temp[i].fir_data);
            printf(" %s", temp1);
            memcpy((blk + 4 * count), temp1, 4);
        }
    }
    Fill_in(blk, count);
    printf("\n*储存在:%d.blk\n", address);
    writeBlockToDisk(blk, address, buf);
    freeBlockInBuffer(blk, buf);
}

//
//Nest-Loop- Join
//
void Nest_Loop_Join(Buffer *buf)
{
    unsigned char *blk1, *blk2, *write;
    char tempr_R[5], temps_R[5], tempr_W[5], temps_W[5], temp_add[5];
    int number = 0, address = 450, capacity = 0, flag = 0;
    for (int r1 = 1; r1 < 17; r1++)
    {
        blk1 = readBlockFromDisk(r1, buf);
        for (int r2 = 0; r2 < 7; r2++)
        {
            memcpy(tempr_R, (blk1 + 8 * r2), 4);
            for (int s1 = 20; s1 < 52; s1++)
            {
                blk2 = readBlockFromDisk(s1, buf);
                for (int s2 = 0; s2 < 7; s2++)
                {
                    memcpy(temps_R, (blk2 + s2 * 8), 4);
                    if (strcmp(tempr_R, temps_R) == 0)
                    {
                        flag = 0;
                        memcpy(tempr_W, (blk1 + r2 * 8 + 4), 4);
                        memcpy(temps_W, (blk2 + s2 * 8 + 4), 4);
                        number++;
                        if (capacity == 0)
                            write = getNewBlockInBuffer(buf);
                        memcpy((write + capacity), tempr_R, 4);
                        memcpy((write + 4 + capacity), tempr_W, 4);
                        memcpy((write + 4 * 2 + capacity), temps_W, 4);
                        printf("(%s, %s, %s)\n", tempr_R, tempr_W, temps_W);
                        capacity = capacity + 12;
                        if (r1 == 16 && s1 == 51)
                        {
                            writeBlockToDisk(write, address, buf);
                            printf("*储存在:%d.blk\n", address);
                            freeBlockInBuffer(write, buf);
                        }
                        else
                        {
                            if (capacity == 60)
                            {
                                flag = 1;
                                sprintf(temp_add, "%u", address + 1);
                                memcpy((write + capacity), temp_add, 4);
                                writeBlockToDisk(write, address, buf);
                                printf("*储存在:%d.blk\n", address);
                                capacity = 0;
                                address++;
                                freeBlockInBuffer(write, buf);
                            }
                        }
                    }
                }
                freeBlockInBuffer(blk2, buf);
            }
        }
        freeBlockInBuffer(blk1, buf);
    }
    if (flag == 0)
    {
        //最后凑不够5条数据直接存并将next_address置为0
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int i = capacity; i < 64; i++)
            memcpy((write + i), Nul, 1);
        writeBlockToDisk(write, address, buf);
        printf("*储存在:%d.blk\n", address);
        freeBlockInBuffer(write, buf);
    }
}

//
//Sort-Merge-Join
//
void Sort_Merge_Join(Buffer *buf, Relation *tempR, Relation *tempS)
{
    unsigned char *write;
    char tempR_fir[5], tempR_sec[5], tempS_sec[5], temp_add[5];
    int total = 0;
    int number = 0, address = 530, capacity = 0, R_fir, S_fir, flag = 0;
    for (int i = 0; i < 224; i++)
    {
        S_fir = tempS[i].fir_data;
        for (int j = 0; j < 112; j++)
        {
            R_fir = tempR[j].fir_data;
            if (S_fir == R_fir)
            {
                flag = 0;
                sprintf(tempR_fir, "%d", R_fir);
                sprintf(tempR_sec, "%d", tempR[j].sec_data);
                sprintf(tempS_sec, "%d", tempS[i].sec_data);
                number++;
                if (capacity == 0)
                    write = getNewBlockInBuffer(buf);
                memcpy((write + capacity), tempR_fir, 4);
                memcpy((write + 4 + capacity), tempR_sec, 4);
                memcpy((write + 4 * 2 + capacity), tempS_sec, 4);
                printf("(%s, %s, %s)\n", tempR_fir, tempR_sec, tempS_sec);
                total++;
                capacity = capacity + 12;
                if (i == 223 && j == 111)
                {
                    writeBlockToDisk(write, address, buf);
                    printf("*储存在:%d.blk\n", address);
                    freeBlockInBuffer(write, buf);
                }
                else
                {
                    if (capacity == 60)
                    {
                        flag = 1;
                        sprintf(temp_add, "%u", address + 1);
                        memcpy((write + capacity), temp_add, 4);
                        writeBlockToDisk(write, address, buf);
                        printf("*储存在:%d.blk\n", address);
                        capacity = 0;
                        address++;
                        freeBlockInBuffer(write, buf);
                    }
                }
            }
        }
    }
    if (flag == 0)
    {
        //最后凑不够5条数据直接存并将next_address置为0
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int i = capacity; i < 64; i++)
            memcpy((write + i), Nul, 1);
        writeBlockToDisk(write, address, buf);
        printf("*储存在:%d.blk\n", address);
        freeBlockInBuffer(write, buf);
    }
    printf("连接次数为%d\n", total);
}

//
//Hash-Join
//
Relation **Hash_Function(Buffer *buf, int number, int *Hash_tum, int address)
{
    int i, n, hash;
    int size[5] = {0};
    char temp[5], temp1[5], temp2[5];
    unsigned char *blk;
    Relation **Hash_table = (Relation **)malloc(5 * sizeof(Relation *));
    for (i = 0; i < 5; i++)
        Hash_table[i] = (Relation *)malloc(number * 7 * sizeof(Relation));
    for (i = 0; i < number; i++)
    {
        blk = readBlockFromDisk(address + i, buf);
        for (int j = 0; j < 7; j++)
        {
            memcpy(temp, (blk + 8 * j), 4);
            hash = atoi(temp) % 5;
            memcpy(temp1, (blk + j * 8), 4);
            memcpy(temp2, (blk + j * 8 + 4), 4);
            n = size[hash];
            Hash_table[hash][n].blk = 0;
            Hash_table[hash][n].fir_data = atoi(temp1);
            Hash_table[hash][n].sec_data = atoi(temp2);
            size[hash]++;
        }
        freeBlockInBuffer(blk, buf);
    }
    for (i = 0; i < 5; i++)
    {
        Hash_tum[i] = size[i];
    }
    return Hash_table;
}
void Hash_Join(Buffer *buf, Relation *tempS)
{
    int address = 600, hash, flag = 0, capacity = 0, number = 0;
    char tempR_fir[5], tempR_sec[5], tempS_sec[5], temp_add[5];
    int *Hash_tum = (int *)malloc(5 * sizeof(int));
    Relation **Hash_table = Hash_Function(buf, 16, Hash_tum, 1);
    unsigned char *write;
    write = getNewBlockInBuffer(buf);
    for (int i = 0; i < 5; i++)
    {
        hash = Hash_tum[i];
        for (int j = 0; j < hash; j++)
        {
            for (int k = 0; k < 223; k++)
            {
                if ((tempS[k].fir_data % 5) == i)
                {
                    if (Hash_table[i][j].fir_data == tempS[k].fir_data)
                    {
                        flag = 0;
                        sprintf(tempR_fir, "%d", Hash_table[i][j].fir_data);
                        sprintf(tempR_sec, "%d", Hash_table[i][j].sec_data);
                        sprintf(tempS_sec, "%d", tempS[k].sec_data);
                        number++;
                        if (capacity == 0)
                            write = getNewBlockInBuffer(buf);
                        memcpy((write + capacity), tempR_fir, 4);
                        memcpy((write + 4 + capacity), tempR_sec, 4);
                        memcpy((write + 4 * 2 + capacity), tempS_sec, 4);
                        printf("(%s, %s, %s)\n", tempR_fir, tempR_sec, tempS_sec);
                        capacity = capacity + 12;
                        if (capacity == 60)
                        {
                            flag = 1;
                            sprintf(temp_add, "%u", address + 1);
                            memcpy((write + capacity), temp_add, 4);
                            writeBlockToDisk(write, address, buf);
                            printf("*储存在:%d.blk\n", address);
                            capacity = 0;
                            address++;
                            freeBlockInBuffer(write, buf);
                        }
                    }
                }
            }
        }
    }
    if (flag == 0)
    {
        //最后凑不够5条数据直接存并将next_address置为0
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int i = capacity; i < 64; i++)
            memcpy((write + i), Nul, 1);
        writeBlockToDisk(write, address, buf);
        printf("*储存在:%d.blk\n", address);
        freeBlockInBuffer(write, buf);
    }
}
//
//连接菜单
//
void CONNECT(Buffer *buf, Relation *tempR, Relation *tempS)
{
    int choose = -1;
    while (1)
    {
        printf("=======================\n");
        printf("请选择连接操作算法:\n");
        printf(" 1.Nest-Loop- Join算法\n");
        printf(" 2.Sort-Merge-Join算法\n");
        printf(" 3.Hash-Join算法\n");
        printf(" 0.返回主菜单\n");
        printf("=======================\n");
        scanf("%d", &choose);
        switch (choose)
        {
        case 1:
            printf("Nest-Loop- Join算法，结果如下;\n------------\nR.A  R.B  S.D:\n");
            Nest_Loop_Join(buf);
            printf("------------\n");
            break;
        case 2:
            printf("Sort-Merge-Join算法，结果如下;\n------------\nR.A  R.B  S.D:\n");
            Sort_Merge_Join(buf, tempR, tempS);
            printf("------------\n");
            break;
        case 3:
            printf("Hash-Join算法，结果如下;\n------------\nR.A  R.B  S.D:\n");
            Hash_Join(buf, tempS);
            printf("------------\n");
            break;
        case 0:
            main();
            break;
        default:
            printf("请输入0-3选择功能!");
            break;
        }
    }
}

//找到R S相同的元组
//则并只要R S整合后减去其中相同的元组即可
//  交即是所有相同元祖的集合
void Find_SameTuple(Buffer *buf, Relation *same)
{
    int i, j, k, l;
    char R_fir[5], R_sec[5], S_fir[5], S_sec[5];
    unsigned char *blk, *next_blk;
    for (i = 1; i < 17; i++)
    {
        blk = readBlockFromDisk(i, buf);
        for (j = 0; j < 7; j++)
        {
            memcpy(R_fir, (blk + 8 * j), 4);
            memcpy(R_sec, (blk + 8 * j + 4), 4);
            if (atoi(R_fir) == 22 && atoi(R_sec) == 712)
                printf("here!  %d %d \n", i, j);
            for (k = 0; k < 32; k++)
            {
                next_blk = readBlockFromDisk(k + 17, buf);
                for (l = 0; l < 7; l++)
                {
                    memcpy(S_fir, (next_blk + 8 * l), 4);
                    memcpy(S_sec, (next_blk + 8 * l + 4), 4);
                    if (atoi(R_fir) == atoi(S_fir) && atoi(R_sec) == atoi(S_sec))
                    {
                        same[same_number].fir_data = atoi(S_fir);
                        same[same_number].sec_data = atoi(S_sec);
                        same_number++;
                    }
                }
                freeBlockInBuffer(next_blk, buf);
            }
        }
        freeBlockInBuffer(blk, buf);
    }
}

//
//并
//
void Parallel(Buffer *buf, Relation *same)
{
    unsigned char *blk, *write;
    int i, j, flag = 0, number = 0, address = 700, write_judge = 0;
    int total = 0;
    char temp1[5], temp2[5], fir[5], sec[5], temp_add[5];
    for (i = 1; i < 17; i++)
    {
        blk = readBlockFromDisk(i, buf);
        write = getNewBlockInBuffer(buf);
        for (j = 0; j < 7; j++)
        {
            write_judge = 0;
            memcpy(temp1, (blk + 8 * j), 4);
            memcpy(temp2, (blk + 8 * j + 4), 4);
            memcpy((write + 8 * number), (blk + 8 * j), 4);
            memcpy((write + 8 * number + 4), (blk + 8 * j + 4), 4);
            printf("(%s, %s)", temp1, temp2);
            fflush(stdout);
            total++;
            number++;
        }
        if (number == 7)
        {
            write_judge = 1;
            printf("\n");
            sprintf(temp_add, "%u", address + 1);
            memcpy((write + 8 * number), temp_add, 4);
            writeBlockToDisk(write, address, buf);
            printf("储存到：%d.blk\n", address);
            address++;
            freeBlockInBuffer(write, buf);
            write = getNewBlockInBuffer(buf);
            number = 0;
        }
        freeBlockInBuffer(blk, buf);
    }
    for (i = 0; i < 32; i++)
    {
        blk = readBlockFromDisk(i + 17, buf);
        //        write=getNewBlockInBuffer(buf);
        for (j = 0; j < 7; j++)
        {
            flag = 0;
            memcpy(fir, (blk + 8 * j), 4);
            memcpy(sec, (blk + 8 * j + 4), 4);
            for (int l = 0; l < same_number; l++)
            {
                sprintf(temp1, "%u", same[l].fir_data);
                sprintf(temp2, "%u", same[l].sec_data);
                if (atoi(fir) == atoi(temp1) && atoi(sec) == atoi(temp2))
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0)
            {
                write_judge = 0;
                memcpy((write + 8 * number), fir, 4);
                memcpy((write + 8 * number + 4), sec, 4);
                printf("(%s, %s)", fir, sec);
                fflush(stdout);
                total++;
                number++;
            }
            if (number == 7)
            {
                write_judge = 1;
                printf("\n");
                sprintf(temp_add, "%u", address + 1);
                memcpy((write + 8 * number), temp_add, 4);
                writeBlockToDisk(write, address, buf);
                printf("储存到：%d.blk\n", address);
                address++;
                write = getNewBlockInBuffer(buf);
                number = 0;
            }
        }
        freeBlockInBuffer(write, buf);
        freeBlockInBuffer(blk, buf);
    }
    printf("并运算一共有：%d个元组", total);
    if (write_judge == 0)
    {
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int m = number * 8; m < 64; m++)
            memcpy((write + m), Nul, 1);
        writeBlockToDisk(write, address, buf);
        printf("\n储存到：%d.blk\n", address);
        freeBlockInBuffer(write, buf);
    }
}

//
//交
//
void Intersection(Buffer *buf, Relation *same)
{
    int i, address = 800, number = 0, flag = 0;
    unsigned char *write = NULL;
    char temp1[5], temp2[5], temp_add[5];
    if (same_number != 0)
    {
        write = getNewBlockInBuffer(buf);
        for (i = 0; i < same_number; i++)
        {
            flag = 0;
            sprintf(temp1, "%d", same[i].fir_data);
            sprintf(temp2, "%d", same[i].sec_data);
            printf("(%s, %s)\n", temp1, temp2);
            memcpy((write + 8 * number), temp1, 4);
            memcpy((write + 8 * number + 4), temp2, 4);
            number++;
            if (number == 7)
            {
                flag = 1;
                sprintf(temp_add, "%u", address + 1);
                memcpy((write + 8 * number), temp_add, 4);
                writeBlockToDisk(write, address, buf);
                printf("储存到：%d.blk\n", address);
                address++;
                freeBlockInBuffer(write, buf);
                write = getNewBlockInBuffer(buf);
                number = 0;
            }
        }
        if (flag == 0)
        {
            char *Nul = (char *)malloc(1 * sizeof(char));
            sprintf(Nul, "");
            for (int m = number * 8; m < 64; m++)
                memcpy((write + m), Nul, 1);
            writeBlockToDisk(write, address, buf);
            printf("储存到：%d.blk\n", address);
            freeBlockInBuffer(write, buf);
        }
        printf("交集一共有%d个元组\n", same_number);
    }
    else
        printf("交集为空!\n");
}

//
//差
//
void Difference(Buffer *buf, Relation *same, int choose)
{
    int address = 0, mark = 1, number = 0, num = 0, write_judge = 0;
    char fir[5], sec[5], temp1[5], temp2[5], temp_add[5];
    unsigned char *blk, *write;
    int total = 0;
    if (choose == 1)
    {
        num = 16;
        address = 850;
    }
    else
    {
        num = 32;
        address = 900;
    }
    for (int i = 0; i < num; i++)
    {
        if (num == 16)
            blk = readBlockFromDisk(i + 1, buf);
        if (num == 32)
            blk = readBlockFromDisk(i + 17, buf);
        write = getNewBlockInBuffer(buf);
        for (int j = 0; j < 7; j++)
        {
            mark = 1;
            memcpy(fir, (blk + 8 * j), 4);
            memcpy(sec, (blk + 8 * j + 4), 4);
            for (int k = 0; k < same_number; k++)
            {
                sprintf(temp1, "%u", same[k].fir_data);
                sprintf(temp2, "%u", same[k].sec_data);
                if (atoi(fir) == atoi(temp1) && atoi(sec) == atoi(temp2))
                {
                    mark = 0;
                    break;
                }
            }
            if (mark == 1)
            {
                write_judge = 0;
                memcpy((write + 8 * number), fir, 4);
                memcpy((write + 8 * number + 4), sec, 4);
                // fir[4]='\0';
                // sec[4]='\0';
                printf("(%s, %s)", fir, sec);
                fflush(stdout);
                number++;
                total++;
            }
            if (number == 7) //最后一块写下一块地址
            {
                write_judge = 1;
                printf("\n");
                sprintf(temp_add, "%u", address + 1);
                memcpy((write + 8 * number), temp_add, 4);
                writeBlockToDisk(write, address, buf);
                printf("储存到：%d.blk\n", address);
                address++;
                freeBlockInBuffer(write, buf);
                write = getNewBlockInBuffer(buf);
                number = 0;
            }
        }

        freeBlockInBuffer(blk, buf);
    }
    printf("差一共有  %d个元组", total);
    if (write_judge == 0)
    {
        char *Nul = (char *)malloc(1 * sizeof(char));
        sprintf(Nul, "");
        for (int m = number * 8; m < 64; m++)
            memcpy((write + m), Nul, 1);
        writeBlockToDisk(write, address, buf);
        printf("\n储存到：%d.blk\n", address);
        freeBlockInBuffer(write, buf);
    }
}

//
//集合操作菜单
//
void COMBINE(Buffer *buf)
{
    int choose = -1, Differ_cho = 0;
    struct Relation *same = (struct Relation *)malloc(sizeof(Relation) * 50);
    Find_SameTuple(buf, same);
    while (1)
    {
        printf("\n=======================\n");
        printf("请选择集合操作算法:\n");
        printf("   1.并\n");
        printf("   2.交\n");
        printf("   3.差\n");
        printf("   0.返回主菜单\n");
        printf("=======================\n");
        //        for (int i = 0; i < same_number; ++i) {
        //            printf("(%d,%d,%d)\n",same[i].fir_data,same[i].sec_data,same[i].blk);
        //        }
        scanf("%d", &choose);

        switch (choose)
        {
        case 1:
            printf("并操作,结果如下;\n------------\n");
            Parallel(buf, same);
            printf("------------\n");
            break;
        case 2:
            printf("交操作,结果如下;\n------------\n");
            Intersection(buf, same);
            printf("------------\n");
            break;
        case 3:
            printf("=======================\n");
            printf("请输入差选择:\n 1.R-S  2.S-R :\n");
            scanf("%d", &Differ_cho);
            if (Differ_cho == 1 || Differ_cho == 2)
            {
                printf("差操作,结果如下;\n------------\n");
                Difference(buf, same, Differ_cho);
                printf("------------\n");
            }
            else
                printf("请输入1或2!");
            break;
        case 0:
            main();
            break;
        default:
            printf("请输入0-3选择功能!");
            break;
        }
    }
}
int **Blk_to_array(char *blk)
{
    char fir[5], sec[5];
    int i, j;
    int **temp;
    temp = (int **)malloc(7 * sizeof(int *));
    for (i = 0; i < 7; i++)
    {
        temp[i] = (int *)malloc(2 * sizeof(int *));
        memcpy(fir, blk + 8 * i, 4);
        memcpy(sec, blk + 8 * i + 4, 4);
        int f1 = atoi(fir);
        int f2 = atoi(sec);
        temp[i][0] = f1;
        temp[i][1] = f2;
    }
    return temp;
}
char *array_to_blk(int temp[7][2], Buffer *buf, int address)
{
    char *blk = getNewBlockInBuffer(buf);
    char fir[5], sec[5];
    int i;
    for (i = 0; i < 7; i++)
    {
        sprintf(fir, "%u", temp[i][0]);
        sprintf(sec, "%u", temp[i][1]);
        memcpy(blk + 8 * i, fir, 4);
        memcpy(blk + 8 * i + 4, sec, 4);
    }
    char next[5];
    sprintf(next, "%u", address);
    memcpy(blk + 56, next, 4);
    return blk;
}
//
//读入从address开始的八块56个元组进行内排序
//
int **base_sort(int address, Buffer *buf)
{
    char fir[5], sec[5];
    int i, j;
    int **eight_blks;
    int buffer_num = 0;
    int **temp;
    char *blk;
    eight_blks = (int **)malloc(56 * sizeof(int *));
    for (i = 0; i < 56; i++)
    {
        eight_blks[i] = (int *)malloc(2 * sizeof(int *));
    }
    for (i = 1; i <= 8; i++)
    {
        blk = readBlockFromDisk(i + address, buf);
        temp = Blk_to_array(blk);
        for (j = 0; j < 7; j++)
        {
            eight_blks[buffer_num][0] = temp[j][0];
            eight_blks[buffer_num][1] = temp[j][1];
            buffer_num++;
        }
        freeBlockInBuffer(blk, buf);
    }
    int tmp1, tmp2;
    for (i = 0; i < 56; i++)
    {
        for (j = 0; j < 56 - i - 1; j++)
        {
            if (eight_blks[j][0] > eight_blks[j + 1][0])
            {
                tmp1 = eight_blks[j][0];
                tmp2 = eight_blks[j][1];
                eight_blks[j][0] = eight_blks[j + 1][0];
                eight_blks[j][1] = eight_blks[j + 1][1];
                eight_blks[j + 1][0] = tmp1;
                eight_blks[j + 1][1] = tmp2;
            }
        }
    }
    // for (i = 0; i < 56; i++)
    // {
    //     printf("%d    %d\n", eight_blks[i][0], eight_blks[i][1]);
    // }
    return eight_blks;
}
void TPMMS1(Buffer *buf, int choose)
{
    int i, j, k, l;
    int address, address_data1, len;
    unsigned char *blk, *write;
    char temp1[5], temp2[5], fir[5], sec[5], next[5];
    int number = 0;
    if (choose == 1) //R
    {
        address = 0;
        len = 2;
        address_data1 = 1000;
    }
    if (choose == 2) //S
    {
        address = 16;
        len = 4;
        address_data1 = 1016;
    }
    unsigned char *kk;
    for (i = 0; i < len; i++)
    {
        int **order_blk = base_sort(8 * i + address, buf);
        // unsigned char *k;
        // Read(k,i+address,buf);
        write = getNewBlockInBuffer(buf);
        for (j = 0; j < 56; j++)
        {
            if (((j + 1) % 7) != 0)
            { //printf("%d \n",order_blk[j][0]);
                sprintf(temp1, "%u", order_blk[j][0]);
                sprintf(temp2, "%u", order_blk[j][1]);
                memcpy(write + 8 * (j % 7), temp1, 4);
                memcpy(write + 8 * (j % 7) + 4, temp2, 4);
                number++;
                // Fill_in(write,j);}
            }
            else if (((j + 1) % 7) == 0 && i != len - 1 || ((j + 1) % 7) == 0 && i == len - 1 && j != 55)
            {
                sprintf(temp1, "%u", order_blk[j][0]);
                sprintf(temp2, "%u", order_blk[j][1]);
                memcpy(write + 8 * (j % 7), temp1, 4);
                memcpy(write + 8 * (j % 7) + 4, temp2, 4);
                sprintf(next, "%u", address_data1 + (j + 1) / 7 + 1 + i * 8);
                memcpy((write + 8 * 7), next, 4);
                writeBlockToDisk(write, address_data1 + (j + 1) / 7 + i * 8, buf);
                Read(kk, address_data1 + (j + 1) / 7 + i * 8, buf);
                freeBlockInBuffer(write, buf);
                write = getNewBlockInBuffer(buf);
                number = 0;
            }
            else if (j == 55 && i == len - 1)
            {
                sprintf(temp1, "%u", order_blk[j][0]);
                sprintf(temp2, "%u", order_blk[j][1]);
                memcpy(write + 8 * (j % 7), temp1, 4);
                memcpy(write + 8 * (j % 7) + 4, temp2, 4);
                sprintf(next, "%u", 0);
                memcpy((write + 8 * 7), next, 4);
                writeBlockToDisk(write, address_data1 + (j + 1) / 7 + i * 8, buf);
                Read(kk, address_data1 + (j + 1) / 7 + i * 8, buf);
                freeBlockInBuffer(write, buf);
                write = getNewBlockInBuffer(buf);
                number = 0;
            }
        }
        //Read(kk, address_data1 + 16, buf);
    }
}
void TPMMS2(Buffer *buf, int choose, int address1, int address2, int address_data, int length)
{
    if (choose == 1)
    {
        unsigned char *blk;
        int **tmp1, **tmp2; //tmp1,tmp2用来存当前的
        int order[7][2];
        int order_num = 0, lasti = 5000, lastj = 5000;
        int flag1 = 1, flag2 = 1;
        int i = 0, j = 0;
        while (i < length && j < length)
        {
            if (i % 7 == 0 && flag1 == 1)
            {
                flag1 = 0;
                blk = readBlockFromDisk(address1, buf);
                tmp1 = Blk_to_array(blk);
                freeBlockInBuffer(blk, buf);
                address1++;
            }
            if (j % 7 == 0 && flag2 == 1)
            {
                flag2 = 0;
                blk = readBlockFromDisk(address2, buf);
                tmp2 = Blk_to_array(blk);
                freeBlockInBuffer(blk, buf);
                address2++;
            }
            if (tmp1[i % 7][0] <= tmp2[j % 7][0])
            {
                order[order_num][0] = tmp1[i % 7][0];
                order[order_num][1] = tmp1[i % 7][1];
                order_num++;
                i++;
                flag1 = 1;
            }
            else
            {
                order[order_num][0] = tmp2[j % 7][0];
                order[order_num][1] = tmp2[j % 7][1];
                order_num++;
                j++;
                flag2 = 1;
            }
            if (order_num == 7)
            {
                char *write = array_to_blk(order, buf, address_data + 1);
                writeBlockToDisk(write, address_data, buf);
                Read(write, address_data, buf);
                address_data++;
                freeBlockInBuffer(write, buf);
                order_num = 0;
            }
        }
        flag1 = 1;
        flag2 = 1;
        if (i == length) //如果i=0那么就把tmp2 剩下的全弄进去
        {
            for (; j <= length; j++)
            {
                if (j % 7 == 0 && flag2 == 1&& j!=length)
                {
                    flag2 = 0;
                    blk = readBlockFromDisk(address2, buf);
                    tmp2 = Blk_to_array(blk);
                    freeBlockInBuffer(blk, buf);
                    address2++;
                }
                if (order_num == 7)
                {
                    char *write = array_to_blk(order, buf, address_data + 1);
                    writeBlockToDisk(write, address_data, buf);
                    Read(write, address_data, buf);
                    address_data++;
                    freeBlockInBuffer(write, buf);
                    order_num = 0;
                }
                else
                {
                    order[order_num][0] = tmp2[j % 7][0];
                    order[order_num][1] = tmp2[j % 7][1];
                    order_num++;
                }
            }
        }
        else if (j == length) //如果i=0那么就把tmp2 剩下的全弄进去
        {
            for (; i <= length; i++)
            {
                if (i % 7 == 0 && flag1 == 1 && i!=length)
                {
                    flag1 = 0;
                    blk = readBlockFromDisk(address1, buf);
                    tmp1 = Blk_to_array(blk);
                    freeBlockInBuffer(blk, buf);
                    address1++;
                }
                if (order_num == 7)
                {
                    char *write = array_to_blk(order, buf, address_data + 1);
                    writeBlockToDisk(write, address_data, buf);
                    Read(write, address_data, buf);
                    address_data++;
                    freeBlockInBuffer(write, buf);
                    order_num = 0;
                }
                else
                {
                    order[order_num][0] = tmp1[i % 7][0];
                    order[order_num][1] = tmp1[i % 7][1];
                    order_num++;
                }
            }
        }
    }
}
int main()
{
    int choose = 0, num;
    Buffer buf;
    unsigned char *kkk;
    /* Initialize the buffer */
    if (!initBuffer(520, 64, &buf))
    {
        perror("Buffer is full!\n");
        return -1;
    }
    printf("success ! \n");
    Relation *tempR = ReadData(&buf, 0);
    Relation *tempS = ReadData(&buf, 1);
    tempR = insertion_sort(tempR, 112);
    //    for(int i=0;i<112;i++)
    //    {
    //        printf("(%d  %d)",tempR[i].fir_data,tempR[i].sec_data);
    //    }
    tempS = insertion_sort(tempS, 224);
    printf("success ! 2\n");
    while (1)
    {
        printf("\n========功能菜单========\n");
        printf("========1.选择========\n");
        printf("========2.投影========\n");
        printf("========3.连接========\n");
        printf("========4.集合========\n");
        printf("========5.查看========\n");
        printf("========0.退出========\n");
        scanf("%d", &choose);
        switch (choose)
        {
        case 1:
            SELECT(&buf, tempR, tempS);
            break;
        case 2:
            Projection(&buf, tempR);
            break;
        case 3:
            CONNECT(&buf, tempR, tempS);
            break;
        case 4:
            COMBINE(&buf);
            break;
        case 5:
            printf("请输入要选择查看的块:\n");
            scanf("%d", &num);
            Read(kkk, num, &buf);
            break;
        case 6:
            TPMMS1(&buf, 1);
            TPMMS1(&buf, 2);
            TPMMS2(&buf, 1, 1001, 1009, 1051, 56); //R 1051-1067
            TPMMS2(&buf, 1, 1017, 1025, 1067, 56);
            TPMMS2(&buf, 1, 1033, 1041, 1083, 56);
            TPMMS2(&buf, 1, 1067, 1083, 1101, 112); //S 1101-1132
            break;
        case 0:
            exit(0);
        default:
            printf("请输入1-4选择功能!");
            break;
        }
    }

    printf("\n");
    printf("IO's is %d\n", buf.numIO); /* Check the number of IO's */

    return 0;
}
