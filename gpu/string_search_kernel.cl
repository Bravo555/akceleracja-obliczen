__kernel void string_search(const global char *text,
                            __global char *keyword,
                            const uint keywordSize,
                            global short* partialMatchTable,
                            global uchar *indices)
{
    uint gid = get_global_id(0);

    size_t textStart = gid * keywordSize;
    size_t textEnd = textStart + 2 * keywordSize;

    int currPosInText = textStart;
    int currPosInKeyword = 0;

    while (currPosInText < textEnd)
    {
        if (keyword[currPosInKeyword] == text[currPosInText])
        {
            currPosInText++;
            currPosInKeyword++;
            if (currPosInKeyword == keywordSize && partialMatchTable[keywordSize] != -1)
            {
                indices[currPosInText - currPosInKeyword] = 1;
                currPosInKeyword = partialMatchTable[currPosInKeyword];
            }
        }
        else
        {
            currPosInKeyword = partialMatchTable[currPosInKeyword];
            if (currPosInKeyword < 0)
            {
                currPosInText++;
                currPosInKeyword++;
            }
        }
    }
}
