#!/bin/bash

#SBATCH --job-name=piplus
#SBATCH --ntasks=1
#SBATCH --cpus-per-task=1
#SBATCH --mem=5G
#SBATCH --time=48:00:00
#SBATCH --array=0-256

#SBATCH --output=/sciclone/scr30/jgiroux/GlueXCampaign0/logs/%x_%A_%a.out
#SBATCH --error=/sciclone/scr30/jgiroux/GlueXCampaign0/logs/%x_%A_%a.out

set -e


GLUEX_TOP=/sciclone/home/jgiroux/gluex_top

SIF=/sciclone/data10/jgiroux/Genesis/gluex_almalinux_9.sif

SIMDIR=/sciclone/scr30/jgiroux/GlueXCampaign0

SQLDIR=/sciclone/data10/jgiroux/Genesis

RUN_LIST=${GLUEX_TOP}/runs_to_index_unique.txt


# ------------------------------------------------------------
# Get run number from array index
# ------------------------------------------------------------

RUN_NUMBER=$(sed -n "$((SLURM_ARRAY_TASK_ID+1))p" "${RUN_LIST}")


if [ -z "${RUN_NUMBER}" ]; then
    echo "ERROR: No run number found"
    echo "Array index:"
    echo "${SLURM_ARRAY_TASK_ID}"
    exit 1
fi


echo "========================================"
echo "Array task:"
echo "${SLURM_ARRAY_TASK_ID}"

echo "Run number:"
echo "${RUN_NUMBER}"
echo "========================================"



# ------------------------------------------------------------
# Run container
# ------------------------------------------------------------

singularity exec \
    --bind ${GLUEX_TOP}:/gluex_top \
    --bind ${SIMDIR}:/sim \
    --bind ${SQLDIR}:${SQLDIR} \
    --env BUILD_SCRIPTS=/gluex_top/build_scripts \
    ${SIF} \
    bash -c "
        source ~/.bashrc
        /gluex_top/run_charged_simulation.sh \
            piplus_campaign0_${RUN_NUMBER} \
            ${RUN_NUMBER} \
            100000 \
	    /gluex_top/config_files/particle_gun_charged.config
    "


echo "Slurm task finished"
