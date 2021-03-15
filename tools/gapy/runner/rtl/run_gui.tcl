if [info exists env(GAPY_SVCF_FILE)] {
    foreach file $env(GAPY_SVCF_FILE) {
        simvision -input $file
    }
}
