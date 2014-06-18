#!/usr/bin/Rscript

# hard codes training iterations (burn in) to 2000

options(max.print=1E8)

args <- commandArgs(TRUE)

if(length(args) != 8  || !file.exists(args[1]) || !file.exists(args[2])  
   || file.exists(args[6]) || !file.exists(args[8]) ){
  cat (length(args), file.exists(args[1]), file.exists(args[2]), file.exists(args[6]));
  cat ("\nMatrix: ", args[1]);
  cat ("\nVocab: ", args[2]);
  cat ("\n# Topics: ", args[3]);
  cat ("\nAlpha: ", args[4]);
  cat ("\nBeta: ", args[5]);
  cat ("\nProject Name: ", args[6]);
  cat ("\n.");
  stop("Usage: ", args[0], " <matrix> <vocab> <# topics> <alpha> <beta> <output file> <iterations> <docmapping file>")
}

library(lda)
docs<-dget(args[1])
vocab<-dget(args[2])
topicParam<-as.double(args[3])
alphaParam<-as.double(args[4])
betaParam<-as.double(args[5]);
outFile<-args[6];
iter<-args[7];
mapping<-as.matrix(read.csv(file=args[8], header=F))

#Set seed so results are reproducable
# set.seed(23333)

#Fit an LDA model
lda.model <- lda.collapsed.gibbs.sampler(docs, topicParam, vocab, iter,
                                         alphaParam, betaParam,burnin=2000)

#Topic-Word distributions conditioned on sample 
# (see "Probabilistic Topic Modesl," Mark Steyvers and Tom Griffiths)
topic.proportions<-((lda.model$topics+betaParam)/(rowSums(lda.model$topics)+(betaParam*length(vocab))))
topic.proportions[is.na(topic.proportions)] <-  1/length(vocab)

#Add row labels
rownames(topic.proportions)<-paste("Topic #",seq(1:topicParam),sep="")
topic.proportions<-as.data.frame(topic.proportions)

#Print data
print(topic.proportions)

#Document-Topic distributions conditioned on sample 
# (see "Probabilistic Topic Modesl," Mark Steyvers and Tom Griffiths)
# "document_expects" aggregates the results while to use the results of the last iteration use "document_sums".  
# see the documentation http://cran.r-project.org/web/packages/lda/lda.pdf.
# the way in which R summs the array for document_expects is potentially problimatic
# (for example, if a toics evolves under a differnt topic number)
# better to average seperate runs.
##topic.proportions <- as.matrix(t(lda.model$document_expects+alphaParam) / (colSums(lda.model$document_expects)+(topicParam*alphaParam)))
topic.proportions <- as.matrix(t(lda.model$document_sums+alphaParam) / (colSums(lda.model$document_sums)+(topicParam*alphaParam)))
topic.proportions[is.na(topic.proportions)] <-  1/topicParam

#Add column and row labels
rownames(topic.proportions)<-mapping
colnames(topic.proportions)<-paste("Topic #",seq(1:topicParam),sep="")
topic.proportions<-as.data.frame(topic.proportions)

#Print document-topic distributions
print(topic.proportions)

#Save model
ldaData<-paste(outFile, ".lda.model", sep="")
dput(lda.model, ldaData)

#Save topic Proportions
topicData<-paste(outFile, ".topic.proportions", sep="")
dput(topic.proportions, topicData)
