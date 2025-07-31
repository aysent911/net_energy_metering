-- CreateTable
CREATE TABLE "Energy" (
    "id" VARCHAR(10) NOT NULL,
    "timestamp" TIMESTAMPTZ(3) NOT NULL,
    "netEnergy" DOUBLE PRECISION NOT NULL,
    "exportEnergy" DOUBLE PRECISION NOT NULL,
    "importEnergy" DOUBLE PRECISION NOT NULL,
    "apparentEnergy" DOUBLE PRECISION NOT NULL
);

-- CreateIndex
CREATE UNIQUE INDEX "Energy_id_key" ON "Energy"("id");
