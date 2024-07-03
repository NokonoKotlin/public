## 自作ウェブサイト
<a href = "https://nokonokotlin.github.io/Home/index.html">こちらのリンク</a> にページを公開しており、主に競技プログラミングの話を中心に記事を掲載しています。他にも情報系の記事や、その他勉強メモもあります。

記事数は $100$ を超えており、中には Qiita などの他プラットフォームに移植しているものもあります。


## 構成や設計


### 主要なフォルダ構成

```
/home : cwd
   |- index.html : トップページ
   |- /static 
         |- /css : 全体で共用の css & home.html 用の css
         |- /js
         |- /image     
   |- /Contents
         |- /static
               |- /css 
               |- /js 
               |- /image 
         |- /algorithm : アルゴリズム記事ページ
         |- /informatics : 情報系記事ページ
         |- /study_memo : 勉強メモ記事ページ
```


