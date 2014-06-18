
library(RColorBrewer)
library(gplots)
x=read.table("matrix.dat", header=TRUE, row.names=1, check.names=FALSE)
foo <- function(x) { sqrt(x) }
x <- foo(x)
mat=data.matrix(x)
# base_size <- 24

png("heatmap.png", height=600, width=600)
heatmap.2(mat,
  Rowv=NA,
  Colv=NA,
  dendrogram= c("none"),
  distfun = dist,
  hclustfun = hclust,
  # xlab = "Topics", ylab = "alpha",
  revC=TRUE,
  key=FALSE,
  keysize=1,
lhei = c(0.05,0.95),
lwid = c(0.15,0.75),
  trace="none",
  density.info=c("none"),
  margins=c(17, 6),
  # col=brewer.pal(10,"PiYG")
  # col= colorpanel(10,"steelblue","white")
  # linecol = "blue",
  cexCol = 1.5 ,
# cexRow = 0.7 ,
# rowsep = c(1,2,3,4,5,6,7,8,9),
# colsep = c(1,2,3,4,5,6,7,8,9),
    # sepcolor="blue",
    # sepwidth=c(0.05,0.05),
  col= colorpanel(10,"lightgray","black")
 # col = cm.colors(256), 


)
dev.off()
