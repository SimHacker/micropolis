using MicropolisEngine;

namespace MicropolisClient
{
    internal class MicropolisGenericEngine : Micropolis
    {
        public MicropolisGenericEngine()
        {
            resourceDir = "res";
            initGame();
        }

        protected string Description { get; set; }

        protected string Title { get; set; }

        public void generateNewMetaCity()
        {
            Title = "New City";
            Description = "A randomly generated city.";
            generateMap();
        }
    }
}