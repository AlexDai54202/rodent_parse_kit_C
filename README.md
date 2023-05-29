# rodent_parse_kit_C


Drag and drop any .UEXP file to the batch script

This will create a "tabular_raw_bytes.csv" file no matter what happens, and if all goes well it should also generate a "tabular_parsed.csv". If any errors occur, please ping me with the file!

I have included ALL uexp files in which you may test.

big_run.py runs all .uexp files through the parser, and creates corresponding files in the csv_outputs folder. THIS WILL NOT REPLACE FILES, PLEASE DELETE THE CONTENTS OF CSV_OUTPUTS IF YOU WANT NEWER FILES.

If you are interested in datamining yourself, please DM me!


Here are the list of common "variable headers" I have learned of:
    A[X]: String with X length (commonly found before a label)
    DA [XXXX]: String with XXXX length (commonly found in long strings)
    [XX]: Integer of X value.
    D1 [XXXX]: Integer of XXXX value.
    D2 [XXXXXXXX]: Integer of XXXXXXXX value
    DC [XXXX]: declare list of XXXX items (commonly found after m_DataList)
    DE [XXXX]: declare 'entity' of XXXX labels (commonly found before a new item, AKA before m_id)
    8[X] : Declare entity of X labels (commonly found in smaller .uexp files)
    CA [XXXX]: FLOAT of XXXX value.
    9X : ARRAY of X length. Each entry consumes 1 byte of space. That byte can contain a header.
