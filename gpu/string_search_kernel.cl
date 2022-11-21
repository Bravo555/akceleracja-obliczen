__kernel void string_search(const global char *text,
                            __global char *keyword,
                            const uint keywordSize,
                            global uchar *indices)
{
    uint gid = get_global_id(0);
    for (int i = 0; i < keywordSize; ++i)
    {
        if (text[gid + i] != keyword[i])
        {
            indices[gid] = false;
            return;
        }
    }
    indices[gid] = true;
};
