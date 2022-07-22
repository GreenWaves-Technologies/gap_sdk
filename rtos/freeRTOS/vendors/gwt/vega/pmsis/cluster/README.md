
# PMSIS Cluster side implementation
## Objectives of this work:
### Create a standard API for code executed on cluster side
    OS independent access to various facilities (dma, event unit, cluster barriers, hw mutex etc)
    Will also eventually include task delegation to FC 
### Implement OS neutral cluster side code
    For code that is only executed on the cluster, add the implementation here as it is, by definition, OS agnostic
    Includes dma, barriers forks and hw mutex (might miss some?)