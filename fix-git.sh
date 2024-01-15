while read p; do
  git checkout temp-trunk $p
done <sel4.files.changed