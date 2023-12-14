#c <- read.table('gisaid_file_c_country_codes.tsv', sep='\t', header=TRUE, quote='') 
#r <- read.table('gisaid_file_r_country_codes.tsv', sep='\t', header=TRUE, quote='') 
library("countrycode")
library("stringr")
library("tidyr")
library("readr")


r <- read_tsv(
  'gisaid_file_r_country_codes.tsv',
  col_names = TRUE,
  col_types = NULL,
  locale = default_locale(),
  na = c("", "NA"),
  quoted_na = TRUE,
  quote = "",
  comment = "",
  trim_ws = FALSE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, Inf),
  skip_empty_rows = TRUE
)

c <- read_tsv(
  'gisaid_file_c_country_codes.tsv',
  col_names = TRUE,
  col_types = NULL,
  locale = default_locale(),
  na = c("", "NA"),
  quoted_na = TRUE,
  quote = "",
  comment = "",
  trim_ws = FALSE,
  skip = 0,
  n_max = Inf,
  guess_max = min(1000, Inf),
  skip_empty_rows = TRUE
)

library(dplyr)

d <- r %>% select("Accession ID", "country_code") %>% full_join(c %>% select("Accession ID", "CountryCode"), by=join_by("Accession ID")) 
d2 <- d %>% filter(country_code != CountryCode)
d2 %>% group_by(country_code, CountryCode) %>% dplyr::summarise(n=n())

