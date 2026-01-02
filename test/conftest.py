from utils import *

@pytest.fixture(scope="session")
def shared_dir():
    overwrite_dir(TEST_RESULT_DIR)
    yield Path(TEST_RESULT_DIR)

@pytest.fixture
def output_dir(shared_dir, request):
    test_dir = shared_dir / request.node.name
    test_dir.mkdir(exist_ok=True)
    return test_dir
