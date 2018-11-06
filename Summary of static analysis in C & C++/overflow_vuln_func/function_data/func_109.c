static guint
fRealTag (tvbuff_t *tvb, proto_tree *tree, guint offset, const gchar *label)
{
    guint8      tag_no, tag_info;
    guint32     lvt;
    guint       tag_len;
    gfloat      f_val;
    proto_item *ti;
    proto_tree *subtree;

    tag_len = fTagHeader(tvb, offset, &tag_no, &tag_info, &lvt);
    f_val = tvb_get_ntohieee_float(tvb, offset+tag_len);
    ti = proto_tree_add_text(tree, tvb, offset, 4+tag_len,
        "%s%f (Real)", label, f_val);
    subtree = proto_item_add_subtree(ti, ett_bacapp_tag);
    fTagHeaderTree(tvb, subtree, offset, &tag_no, &tag_info, &lvt);

    return offset+tag_len+4;
}