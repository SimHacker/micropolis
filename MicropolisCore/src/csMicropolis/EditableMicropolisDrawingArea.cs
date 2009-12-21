namespace MicropolisClient
{
    internal class EditableMicropolisDrawingArea : MicropolisDrawingArea
    {
        private MicropolisCSEngine _engine;

        public EditableMicropolisDrawingArea(MicropolisCSEngine engine)
        {
            _engine = engine;
        }
    }
}