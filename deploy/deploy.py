#!/usr/bin/env python
"""
@file deploy.py
@brief Script for archiving project source files
@author Roman Janiczek
@email "xjanic25@vutbr.cz"
@version 1.0
"""
import tarfile
import os.path

def createArchive():
    """
    @brief Function that archives all files in '/src' folder + rozdeleni, rozsireni and documentation.pdf files
    """
    ## Modifying Makefile to work with the archive
    createCopy = "cp ../Makefile Makefile"                      # Create temp copy of Makefile
    makeModif0 = "sed -i 's+src/++g' Makefile"                  # Remove 'src/' from Makefile
    makeModif1 = "sed -i 's+src++g' Makefile"                   # Remove 'src' from Makefile
    makeModif2 = "sed -i 's+tests/++g' Makefile"                # Remove 'tests/' from Makefile
    os.system(createCopy)
    os.system(makeModif0)
    os.system(makeModif1)
    os.system(makeModif2)
    ## Creating archive
    with tarfile.open("xhorky32.tgz", "w:gz") as tar:
        for i in os.listdir('../src'):                          # Add all source files to the archive
            tar.add('../src/' + i, i)
        tar.add('../ifj2020.c', 'ifj2020.c')                    # Add main file for executable
        tar.add('../ifj2020.h', 'ifj2020.h')                    # Add main file for executable
        tar.add('Makefile', 'Makefile')                         # Add Makefile to the archive
        tar.add('../doc/rozdeleni', 'rozdeleni')                # Add rozdeleni file to the archive
        tar.add('../doc/rozsireni', 'rozsireni')                # Add rozsireni file to the archvie
#        tar.add('../doc/documentation.pdf', 'dokumentace.pdf')  # Add documentation file to the archvie

if __name__ == '__main__':
    createArchive()