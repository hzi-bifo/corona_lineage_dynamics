library(dplyr)
library(tidyr)
library(seqinr)

seq_mutation <- function(s_in, mut_cnt = 3) {
  s <- unlist(strsplit(s_in, ""))
  locs <- sample.int(length(s), mut_cnt)
  locs <- locs[s[locs] != "-"]
  s[locs] <- c("a", "t", "c", "g")[sample.int(4, length(locs), replace = TRUE)]
  paste(s, collapse = "")
}

# Source: https://github.com/corneliusroemer/
#         pango-sequences/blob/main/data/
#         pango-consensus-sequences_genome-nuc.fasta.zst
seqs <- read.fasta("pango-consensus-sequences_genome-nuc.fasta",
                   as.string = TRUE, seqtype = "DNA")
d <- read.csv("metadata.tsv", sep = "\t")

seqs_df <- data.frame()
for (n in names(seqs)) {
  seqs_df <- rbind(seqs_df, data.frame(n, seqs[[n]]));
}

d2 <- left_join(d, seqs_df, by = c("Pango.lineage" = "n")) %>%
  filter(!is.na(seqs..n..))

seqs <- sapply(d2$seqs..n.., seq_mutation)

d2$seqs <- seqs
write.table(d2 %>% filter(!is.na(seqs..n..)) %>%
              select(Accession.ID, seqs), "seqs.fasta",
            quote = FALSE, sep = "\n>", col.names = FALSE,
            row.names = FALSE)
