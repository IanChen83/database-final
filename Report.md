<style>
@import url(http://fonts.googleapis.com/earlyaccess/cwtexhei.css);
.markdown-body{
    font-family: 'cwTeXHei';
}
.markdown-body h1, h2, h3, h4, h5, h6{
	font-family: "Times New Roman"
}
.markdown-body h1{
    text-align: center
}
.markdown-body h4{
    color: #666
}
.markdown-body img{
    border: 1px solid #666;
    text-align:center;
    box-sizing: content-box;
    position: relative;
    display: block;
    margin: 0 auto;
	max-width: 90%;
}
table img{
    margin:0;
    padding: 0;
}
.list_wrapper ol{
    list-style-type: lower-roman;
}

.memberList {
	margin: 0;
}

.memberList p {
	margin: 0;
	line-height: 1.3;
	font-family: monospace;
}

</style>

Database Final Project - B+Tree Indexing
==============

<div class="memberList" style="text-align: right">
<p>
R05921035 陳奕安
</p>
<p>
R05921043 林哲賢
</p>
<p>
R05921037 陳冠廷
</p>
</div>

## Introduction
下圖為我們的架構。parser會將每一行指令轉換成action物件，而main接收到這些物件對B-plus tree和RecordManager操作。這樣的架構便於我們合作開發。

![](https://i.imgur.com/M10XufR.png)

## Implementation Details

#### Input/Command Parsing
關於input/commands的操作都在`/src/action.h`和`/src/action.cpp`裡。我們先對常用的幾個字串處理建立函式（例如，判斷key的值是否正確），然後對每種指令建立validation函式和parsing函式。如此一來，parsing函式就不需要處理輸入格式是否正確的問題，程式可閱讀性能大大增加。雖然對每一個輸入都要經過兩個pass(validation & parsing)，但在效能上幾乎沒有影響。

經過pasing後，每一個input/command都會被轉成一個action。action是一個我們內部定義的溝通物件，因此架構上可以將parsing視為一個獨立的單元。
#### Value
Value其實是指Record的Key，實作部分在`/src/value.h`和`/src/value.cpp`裡面，在value這個struct中定義了兩種valid的type Integer和String用以在後續判斷tree和relation的type，存data的方式是利用一個Union放string或integer data，就可以將兩種不同型態的key包裝在同一個struct下，並且implement各種compare的operator，就可以在tree中輕易的利用Value來做insert或search。
```clike=1
struct Value {
    Value(): type(ValueType::Undefined), IntValue(0) {}
    Value(int v): type(ValueType::Integer), IntValue(v) {}
    Value(char* v): type(ValueType::String), StrValue(v) {}
    ValueType type;
    union {
        int IntValue;
        char* StrValue;
    };
};
```

#### B+Tree

> ※以下所稱node皆為一個index page

B+Tree 的架構implement在`/src/bpt.h`和`/src/bpt.cpp`裡。
node的型態分為兩種leaf跟non_leaf分別都繼承自bplus_node，所以在trace整個tree的時候需要對於node的type 去做判別，每個node會先alloc足夠的memory，並且用一個entry或children去紀錄這個node的children數量，在我的架構中是可以在tree的initialize的階段設定tree的fanout(包括 non_leaf的order和leaf的entry)數量，藉此增加tree的彈性，也可以因應String和Int兩種不同的Key去做調整。我們也在class definition中增加#ifdef，以利於在Unit test中能夠直接取用private data增加test的可靠度。這部分基本上就是implement完所有會用到的command，演算法的部分就不在多贅述。
```clike=1
class bplus_tree {
public:
    bplus_tree(int, int, int, ValueType);
    ~bplus_tree();
    void bplus_tree_dump();
    rid_t bplus_tree_get(Value);
    bool bplus_tree_insert(Value, rid_t);
    rid_t bplus_tree_search(Value key);
    bool bplus_tree_delete(Value);
    vector<rid_t> bplus_tree_get_range(Value, Value);
    //return pair<index, leaf>
    void bplus_tree_deinit();

    // helper
    bplus_leaf* leaf_new();
    bplus_non_leaf* non_leaf_new();
    void non_leaf_delete(bplus_non_leaf*);
    void leaf_delete(bplus_leaf*);

    int get_leaf_num();
    int get_non_leaf_num();


#ifndef UNIT_TEST
private:
#else
public:
#endif
    //// private method
    bool leaf_insert(bplus_leaf *, Value, rid_t);
    bool non_leaf_insert(bplus_non_leaf *, bplus_node*, Value, int);
    void non_leaf_remove(bplus_non_leaf*, int , int );
    bool leaf_remove(bplus_leaf*, Value);

    //// data member
    int order;
    int entries;
    int m_level;
    int non_leaf_num;
    int leaf_num;
    ValueType valueType;
    bplus_node *root;
    bplus_node **head;
};
```

#### Slotted Page

我們根據課程投影片與作業要求，定義page的資料結構。所有關於page的操作都在`/src/page.h`和`/src/page.cpp`。在`/src/page.h`裡可以發現很多基本如何讀取page資料的巨集。這樣的寫法可以讓compiler可以做constant folding優化程式碼，也可以讓這些操作沒有頻繁的function call。

page是一個有512個bytes組成的。page前面的空間放所有的records。從後面看起，最後4 bytes放`free_space_pointer`（只存pointer的最後4 bytes，因此能同時處理32 bits或64 bits的pointer）；接著每4 bytes放從0開始的slot；再接下來是從零開始的每個slot。

page和slot的定義如下。
```clike=1
typedef char Page;

struct Slot {
    int16_t offset;
    uint16_t length;
};
```

當新增record的時候會檢查空間是否足夠。如果空間不足，還會檢查之前刪掉record留下來的空間是否可用。依然不行，回傳`-1`。刪除的時候，本來的`slot offset`會變成`-(slot offset + 1)`，因此我們可以用`offset`是否大於0來記住slot的狀態。

#### Record Manager
一個relation可以包含很多slotted page。為了管理這些slotted page，我們定義一個RecordManager的class在`/src/rm.h`和`/src/rm.cpp`，並宣告一個內部使用的typename`RawData`為`std::pair<char*, unsigned int>`。

RecordManager作為一個抽象層，對外的方法只有三個：`addRecord`、`removeRecord`、及`getRecord`。使用這三個function來包裝對同個relation所有page集合的操作。

#### Relation
單純是讓main可以同時利用一個name存取tree和record manager的class


#### Main
主要handle讀寫檔案和output的format，基本上就是對於每一行parse出來的action去對tree和record manager做相對應的指令並output

## Testing
在這個project裡我們主要使用Google Test來做unit testing。Unit testing在開發處理資料結構的函式的時候可以迅速發現程式不足的地方。所有test都放在`/test/`資料夾下。

以下即是個檢查初始化是否正確的例子。

```clike=1
#include "gtest/gtest.h"

TEST(PageTest, InitPageSpec) {
    Page* page = create_page();
    ASSERT_TRUE(page != NULL);
    EXPECT_EQ(records_in_page(page), 0); 
    EXPECT_EQ(free_space_pointer(page) - page, 0); 
    delete_page(page);
}
```

## Discussion

* **How our implementation would changes if duplicated keys are allowed?**
我們的想法是把原本leaf page的Key-Rid的pair改成Key-(Rid加上一個pointer)，有點類似overflow page的理念，如果pointer是null就是只有一組record對應這個key，如果有值那就去找這個rid的next再繼續找下去直到是null，就可以把這些rid串起來，需要在B+Tree裡多handle一個overflow page去存這些concat的rid，並且需要在滿的時候在alloc memory，這樣我們就可以用每組record多一個pointer的space去換得允許duplicated key的feature。
  